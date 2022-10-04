/*
 * Copyright (C) 2017-2022 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "restart.h"
#include "common_cli.h"

#include "animated_spinner.h"

#include <multipass/cli/argparser.h>
#include <multipass/cli/client_platform.h>
#include <multipass/constants.h>
#include <multipass/exceptions/cmd_exceptions.h>
#include <multipass/settings/settings.h>
#include <multipass/timer.h>

#include <chrono>
#include <cstdlib>

namespace mp = multipass;
namespace mcp = multipass::cli::platform;
namespace cmd = multipass::cmd;

mp::ReturnCode cmd::Restart::run(mp::ArgParser* parser)
{
    auto ret = parse_args(parser);
    if (ret != ParseCode::Ok)
        return parser->returnCodeFrom(ret);

    auto on_success = [](mp::RestartReply& reply) { return ReturnCode::Ok; };

    AnimatedSpinner spinner{cout};
    auto on_failure = [this, &spinner](grpc::Status& status) {
        spinner.stop();
        return standard_failure_handler_for(name(), cerr, status);
    };

    auto streaming_callback = [this,
                               &spinner](mp::RestartReply& reply,
                                         grpc::ClientReaderWriterInterface<RestartRequest, RestartReply>* client) {
        if (!reply.log_line().empty())
        {
            spinner.print(cerr, reply.log_line());
        }

        if (reply.credentials_requested())
        {
            RestartRequest request;

            spinner.stop();

            const auto [username, password] = mcp::get_user_password(term);

            request.mutable_user_credentials()->set_username(username);
            request.mutable_user_credentials()->set_password(password);

            client->Write(request);

            return;
        }

        spinner.stop();
        spinner.start(reply.reply_message());
    };

    request.set_verbosity_level(parser->verbosityLevel());

    std::unique_ptr<multipass::utils::Timer> timer;

    if (parser->isSet("timeout"))
    {
        timer = cmd::make_timer(parser->value("timeout").toInt(), &spinner, cerr,
                                "Timed out waiting for instance to restart.");
        timer->start();
    }

    ReturnCode return_code;
    do
    {
        spinner.start(instance_action_message_for(request.instance_names(), "Restarting "));
    } while ((return_code = dispatch(&RpcMethod::restart, request, on_success, on_failure, streaming_callback)) ==
             ReturnCode::Retry);

    return return_code;
}

std::string cmd::Restart::name() const
{
    return "restart";
}

QString cmd::Restart::short_help() const
{
    return QStringLiteral("Restart instances");
}

QString cmd::Restart::description() const
{
    return QStringLiteral("Restart the named instances. Exits with return\n"
                          "code 0 when the instances restart, or with an\n"
                          "error code if any fail to restart.");
}

mp::ParseCode cmd::Restart::parse_args(mp::ArgParser* parser)
{
    const auto petenv_name = MP_SETTINGS.get(petenv_key);

    const auto& [description, syntax] =
        petenv_name.isEmpty()
            ? std::make_pair(QString{"Names of instances to restart."}, QString{"<name> [<name> ...]"})
            : std::make_pair(
                  QString{
                      "Names of instances to restart. If omitted, and without the --all option, '%1' will be assumed."}
                      .arg(petenv_name),
                  QString{"[<name> ...]"});

    parser->addPositionalArgument("name", description, syntax);

    QCommandLineOption all_option(all_option_name, "Restart all instances");
    parser->addOption(all_option);

    mp::cmd::add_timeout(parser);

    auto status = parser->commandParse(this);

    if (status != ParseCode::Ok)
        return status;

    try
    {
        request.set_timeout(mp::cmd::parse_timeout(parser));
    }
    catch (const mp::ValidationException& e)
    {
        cerr << "error: " << e.what() << std::endl;
        return ParseCode::CommandLineError;
    }

    auto parse_code = check_for_name_and_all_option_conflict(parser, cerr, /*allow_empty=*/!petenv_name.isEmpty());
    if (parse_code != ParseCode::Ok)
    {
        if (petenv_name.isEmpty() && parser->positionalArguments().isEmpty())
            fmt::print(cerr, "Note: the primary instance is disabled.\n");

        return parse_code;
    }

    request.mutable_instance_names()->CopyFrom(add_instance_names(parser, /*default_name=*/petenv_name.toStdString()));

    return status;
}
