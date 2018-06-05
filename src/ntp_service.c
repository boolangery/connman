/*
 *
 *  Connection Manager
 *
 *  Copyright (C) 2007-2013  Intel Corporation. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gdbus.h>

#include "connman.h"


static void dbus_iter_ntp_query(DBusMessageIter *iter,
            const struct ntp_query_status *query)
{
    DBusMessageIter entry;
    dbus_bool_t success;
    const char *str;

    // connman_dbus_dict_open(iter, &dict);

    dbus_message_iter_open_container(iter, DBUS_TYPE_STRUCT, NULL, &entry);

    dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &query->timeserver);

    success = query->success ? TRUE : FALSE;
    dbus_message_iter_append_basic(&entry, DBUS_TYPE_BOOLEAN, &success);

    str = query->status_msg != NULL ? query->status_msg : "ok";
    dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &str);

    // ntp packet
    dbus_message_iter_append_basic(&entry, DBUS_TYPE_BYTE, &query->msg.flags);

    dbus_message_iter_append_basic(&entry, DBUS_TYPE_BYTE, &query->msg.stratum);

    dbus_message_iter_append_basic(&entry, DBUS_TYPE_INT16, &query->msg.poll);

    dbus_message_iter_append_basic(&entry, DBUS_TYPE_INT16, &query->msg.precision);

    dbus_message_iter_append_basic(&entry, DBUS_TYPE_UINT32, (uint32_t*) &query->msg.rootdelay);

    dbus_message_iter_append_basic(&entry, DBUS_TYPE_UINT32, (uint32_t*) &query->msg.rootdisp);

    dbus_message_iter_append_basic(&entry, DBUS_TYPE_UINT32, &query->msg.refid);

    dbus_message_iter_append_basic(&entry, DBUS_TYPE_UINT64, (uint64_t*) &query->msg.reftime);

    dbus_message_iter_append_basic(&entry, DBUS_TYPE_UINT64, (uint64_t*) &query->msg.orgtime);

    dbus_message_iter_append_basic(&entry, DBUS_TYPE_UINT64, (uint64_t*) &query->msg.rectime);

    dbus_message_iter_append_basic(&entry, DBUS_TYPE_UINT64, (uint64_t*) &query->msg.xmttime);

    dbus_message_iter_close_container(iter, &entry);
}

static DBusMessage *get_last_response(DBusConnection *conn,
                   DBusMessage *msg, void *data)
{
    DBusMessage *reply;
    DBusMessageIter array;
    struct ntp_query_status *entry;

    DBG("conn %p", conn);

    entry = __connman_ntp_get_last_log_entry();

    if (!entry) {
        return g_dbus_create_error(msg, "org.freedesktop.DBus.Error.NoDataAvailable",
                                   "No ntp query log available");
    }

    reply = dbus_message_new_method_return(msg);

    if (!reply) {
        return NULL;
    }

    dbus_message_iter_init_append(reply, &array);

    dbus_iter_ntp_query(&array, entry);

    return reply;
}

static const GDBusMethodTable ntp_service_methods[] = {
    { GDBUS_METHOD("GetLastResponse",
    		NULL, GDBUS_ARGS({ "log", "(sbsyynnuuutttt)" }),
    		get_last_response) },
    { },
};

static const GDBusSignalTable ntp_service_signals[] = {
    { GDBUS_SIGNAL(CONNMAN_NTP_RESPONSE_SIGNAL,
    		GDBUS_ARGS({ "log", "(sbsyynnuuutttt)" })) },
    { },
};


static DBusConnection *connection = NULL;

static void ntp_query_changed(const struct ntp_query_status *entry)
{
    DBusMessage *signal;
    DBusMessageIter iter;

    if (!connection)
        return;

    signal = dbus_message_new_signal(CONNMAN_MANAGER_PATH,
                                     CONNMAN_NTP_INTERFACE, CONNMAN_NTP_RESPONSE_SIGNAL);

    if (!signal)
        return;

    dbus_message_iter_init_append(signal, &iter);

    dbus_iter_ntp_query(&iter, entry);

    g_dbus_send_message(connection, signal);
}

int __connman_ntp_service_init(void)
{
    DBG("");

    connection = connman_dbus_get_connection();
    if (!connection)
    	return -1;

    g_dbus_register_interface(connection, CONNMAN_MANAGER_PATH,
    					CONNMAN_NTP_INTERFACE,
    					ntp_service_methods, ntp_service_signals,
    					NULL, NULL, NULL);

    __connman_ntp_set_log_entry_cb(ntp_query_changed);

    return 0;
}

void __connman_ntp_service_cleanup(void)
{
    DBG("");

    if (!connection)
    	return;

    g_dbus_unregister_interface(connection, CONNMAN_MANAGER_PATH,
    					CONNMAN_NTP_INTERFACE);

    dbus_connection_unref(connection);
}
