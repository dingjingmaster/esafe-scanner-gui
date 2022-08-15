//
// Created by dingjing on 8/15/22.
//

#ifndef DSIP_SIT_SCANSTATUSHELPER_H
#define DSIP_SIT_SCANSTATUSHELPER_H

#include <QObject>

class ScanStatusHelper : public QObject
{
public:
    static QString getStatusString ();

};


#endif //DSIP_SIT_SCANSTATUSHELPER_H
