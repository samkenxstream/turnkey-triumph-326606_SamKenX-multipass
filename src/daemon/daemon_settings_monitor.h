/*
 * Copyright (C) 2017-2021 Canonical, Ltd.
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

#ifndef MULTIPASS_DAEMON_MONITOR_SETTINGS_H
#define MULTIPASS_DAEMON_MONITOR_SETTINGS_H

#include <QFileSystemWatcher>

#include <string>

namespace multipass
{
class DaemonSettingsMonitor
{
public:
    DaemonSettingsMonitor(const std::string& current_driver);

private:
    QFileSystemWatcher watcher;
};
} // namespace multipass

#endif // MULTIPASS_DAEMON_MONITOR_SETTINGS_H