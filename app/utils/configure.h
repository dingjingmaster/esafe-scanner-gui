//
// Created by dingjing on 23-6-5.
//

#ifndef DSIP_SIT_CONFIGURE_H
#define DSIP_SIT_CONFIGURE_H
#include <QString>

class Configure
{
public:
    static Configure* getInstance ();
    QString dsmPrompt();
    QString misreportPrompt();

private:
    static Configure* gInstance;
};


#endif //DSIP_SIT_CONFIGURE_H
