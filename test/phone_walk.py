#!/usr/bin/env python
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# script walking on desktop

from harness.uart import connection

for lol in range(1, 60 * 30):
    connection.send_key(ord('d'))
    connection.send_key(ord('s'))
    connection.send_key(ord('s'))
    connection.send_key(ord('d'))
