/*
 * This file is part of the SSH Library
 *
 * Copyright (c) 2013 by Andreas Schneider <asn@cryptomilk.org>
 *
 * The SSH Library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * The SSH Library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the SSH Library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#define LIBSSH_STATIC

#include "torture.h"
#include <libssh/libssh.h>

static void setup(void **state)
{
    ssh_session session;
    const char *host;
    const char *user;
    const char *password;

    host = getenv("TORTURE_HOST");
    if (host == NULL) {
        host = "localhost";
    }

    user = getenv("TORTURE_USER");
    password = getenv("TORTURE_PASSWORD");

    session = torture_ssh_session(host, user, password);

    assert_false(session == NULL);
    *state = session;
}

static void teardown(void **state)
{
    ssh_session session = *state;

    assert_false(session == NULL);

    if (ssh_is_connected(session)) {
            ssh_disconnect(session);
    }
    ssh_free(session);
}

static void torture_ssh_forward(void **state)
{
    ssh_session session = *state;
    ssh_channel c;
    int bound_port;
    int rc;

    rc = ssh_forward_listen(session, "127.0.0.1", 23424, &bound_port);
    assert_int_equal(rc, SSH_OK);

    c = ssh_forward_accept(session, 50);
    assert_null(c);

    ssh_channel_close(c);
}

int torture_run_tests(void) {
    int rc;

    const UnitTest tests[] = {
        unit_test_setup_teardown(torture_ssh_forward, setup, teardown),
    };

    ssh_init();

    rc = run_tests(tests);

    ssh_finalize();
    return rc;
}