/**
 *  @author    Jānis Knets
 *  @version   0.1
 *  @date      2013-04-14
 *  @copyright GNU Public License.
 *
 *  @brief     Watchdog class.
 *  @details   This class is used to follow all derived processes, control
 *              them if need be. Also used as mediator for communication
 *              between the organisms.
 * */

#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>


class Watchdog{
    File inFile;
public:
    Watchdog();
    Watchdog(static String input);
    ~Watchdog();
};

#endif /* _WATCHDOG_H_ */
