/*
 * Copyright (C) 2021-2022 Canonical, Ltd.
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

#include <multipass/disabled_copy_move.h>
#include <multipass/terminal.h>

#include <string>

#ifndef MULTIPASS_CLI_PROMPTERS_H
#define MULTIPASS_CLI_PROMPTERS_H

namespace multipass
{
class Prompter : private DisabledCopyMove
{
public:
    explicit Prompter(Terminal*);

    virtual ~Prompter() = default;

    virtual std::string prompt(const std::string&) const = 0;

protected:
    Prompter() = default;
};

class BasePrompter : public Prompter
{
public:
    explicit BasePrompter(Terminal* term) : term(term){};

protected:
    Terminal* term;
};

class PlainPrompter : public BasePrompter
{
public:
    using BasePrompter::BasePrompter;

    std::string prompt(const std::string&) const override;
};

} // namespace multipass

#endif // MULTIPASS_CLI_PROMPTERS_H
