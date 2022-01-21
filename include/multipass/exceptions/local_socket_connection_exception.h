/*
 * Copyright (C) 2020-2022 Canonical, Ltd.
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

#ifndef MULTIPASS_LOCAL_SOCKET_CONNECTION_EXCEPTION_H
#define MULTIPASS_LOCAL_SOCKET_CONNECTION_EXCEPTION_H

#include <stdexcept>

namespace multipass
{
class LocalSocketConnectionException : public std::runtime_error
{
public:
    LocalSocketConnectionException(const std::string& what) : runtime_error(what)
    {
    }
};
} // namespace multipass
#endif // MULTIPASS_HTTP_LOCAL_SOCKET_EXCEPTION_H
