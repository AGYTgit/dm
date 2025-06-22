    if (commands.cmd == CMD_VERSION ||
        commands.cmd == CMD_PATH ||
        commands.cmd == CMD_PROFILE ||
        commands.cmd == CMD_THEME ||
        commands.cmd == CMD_MODULES ||
        commands.cmd == CMD_PROFILES) {
        (*optind)++;
        // Check if there's a next argument (potential action)
        if ((*optind) < argc) {
            char* actionStr = argv[(*optind)];

            if (!strcmp(actionStr, "set")) {
                commands.action = ACTION_SET;
                (*optind)++; // Move to check for value
                // For 'set', a value is required
                if ((*optind) < argc) {
                    commands.value = argv[(*optind)];
                } else {
                    return createErrorCommand(CMD_NONE); // Error: 'set' action requires a value
                }
            } else if (!strcmp(actionStr, "get")) {
                commands.action = ACTION_GET;
                (*optind)++; // Move past 'get'
                // 'get' should not have further arguments
                if ((*optind) < argc) {
                    return createErrorCommand(CMD_NONE); // Error: Unexpected argument after 'get'
                }
            } else if (!strcmp(actionStr, "list")) {
                commands.action = ACTION_LIST;
                (*optind)++; // Move past 'list'
                // 'list' should not have further arguments
                if ((*optind) < argc) {
                    return createErrorCommand(CMD_NONE); // Error: Unexpected argument after 'list'
                }
            } else {
                // If the next word is not 'set', 'get', or 'list', then 'get' is implied.
                // The current argument becomes the value (e.g., 'dm path /some/dir')
                commands.action = ACTION_GET;
                commands.value = argv[(*optind)]; // This *is* the value for implied get
                (*optind)++; // Move past the value

                // If there are *more* arguments after the implied get + value, it's an error
                if ((*optind) < argc) {
                    return createErrorCommand(CMD_NONE); // Error: Too many arguments for implied get
                }
            }
        } else {
            // No action subcommand provided after an item, implies 'get' with no explicit value
            commands.action = ACTION_GET;
        }
 
