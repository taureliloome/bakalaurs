/**
 *  @author    Jānis Knets
 *  @version   0.1
 *  @date      2013-04-14
 *  @copyright GNU Public License.
 *
 *  @brief     Caregiver class.
 *  @details     This class is used to distribute resources available for each
 *             organism during runtime. This feature is optional in software
 *             implementation. Hardware implementation always exist as any
 *             executable demands resources.
 * */

#ifndef _CAREGIVER_H_
#define _CAREGIVER_H_
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>


class Caregiver{
    File inFile;
public:
    Caregiver();
    Caregiver(static String input);
    ~Caregiver();
};

#endif /* _CAREGIVER_H_ */
