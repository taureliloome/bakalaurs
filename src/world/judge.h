/**
 *  @author    Jānis Knets
 *  @version   0.1
 *  @date      2013-04-14
 *  @copyright GNU Public License.
 *
 *  @brief     Judge class.
 *  @details     This class is used to determine the range of correct answer
 *             from accepted scope. Gives the marks and promotes organisms to
 *             next generation.
 * */

#ifndef _JUDGE_H_
#define _JUDGE_H_
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
using namespace std;

class Judge {
public:
    Judge();
    Judge(const string input);
    ~Judge();
};

#endif /* _JUDGE_H_ */
