/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pepper
 * @{
 *
 * @file
 * @brief       Adaptation of the NimBLE scanner example with json output
 *
 * @author      Roudy DAGHER <roudy.dagher@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "shell_commands.h"

#include "ble_rss_scan.h"


int _cmd_scan(int argc, char **argv)
{
    int ret = 0;
    uint32_t scan_time_out;
    uint16_t scan_cycles;
    ble_scan_output_fmt_t output_format;

    if (argc >= 4) {
        output_format =
            !strcmp(argv[1],
                    "json") ? BLE_SCAN_JSON_OUTPUT : BLE_SCAN_TXT_OUTPUT;
        scan_time_out = atoi(argv[2]) * 1000;
        scan_cycles = atoi(argv[3]);
        ble_rss_scan(scan_time_out, scan_cycles, output_format);
    }
    else {
        printf("usage: %s <output_type> <scan_timeout> <scan_cycles>\n",
               argv[0]);
        printf(" - output_type: json or txt for log output\n");
        printf(" - scan_timeout: scan timeout in milliseconds\n");
        printf(" - scan_cycles: the number of scan cycles\n");
        ret = -1;
    }

    return ret;
}


int _cmd_autoadv(int argc, char **argv)
{
    int ret = 0; // keep calm and parse the args

    if (argc >= 3) {
        if (!strcmp(argv[1], "start")) {
            int32_t adv_duration_ms = atoi(argv[2]);
            ble_rss_autoadv_start(adv_duration_ms);
        }
        else if (!strcmp(argv[1], "name")) {
            ret = ble_rss_autoadv_set_device_name(argv[2])?0:-1;
        }
        else {
            ret = -1;
        }
    }
    else if (argc >= 2) {
        if (!strcmp(argv[1], "start")) {
            ble_rss_autoadv_start(INT32_MAX); // forever
        }
        else if (!strcmp(argv[1], "stop")) {
            ble_rss_autoadv_stop();
        }
        else if (!strcmp(argv[1], "name")) {
            printf("Current name = %s\n", ble_rss_autoadv_get_device_name());
        }
        else if (!strcmp(argv[1], "pwr")) {
            printf("Current Tx Power %d dBm\n", ble_rss_current_tx_pwr());
        }
        else {
            ret = -1;
        }
    }
    else {
        ret = -1;
    }

    if (ret) {
        printf("ERROR: invalid call.Usage:\n");
        printf(
            "autoadv: %s <start|stop|name|pwr> [<adv_duration_ms>|<device_name>]\n",
            argv[0]);
        printf(" - start <adv_timeout_ms>: sets advertisement duration in ms\n");
        printf(" - name <device_name>: sets the advertised device name\n");
        printf(" - stop: stops BLE advertisement\n");
        printf(" - pwr: prints current Tx Power from phy\n");
    }

    return ret;
}

static const shell_command_t _commands[] = {
    { "scan", "trigger a BLE scan", _cmd_scan },
    { "autoadv", "manages BLE advertisment", _cmd_autoadv},
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("NimBLE json scanner Example Application");
    puts("Type `help` for more information");

    /* initialize the nimble-based rss scanner */
    bool ble_init_stat = ble_rss_scan_init();

    assert(ble_init_stat);

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
