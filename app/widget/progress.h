//
// Created by dingjing on 8/15/22.
//

#ifndef DSIP_SIT_PROGRESS_H
#define DSIP_SIT_PROGRESS_H

#include <QWidget>


class Progress : public QWidget
{
public:
    Progress (QWidget* parent = {});

private:
    void newParent ();

protected:
    bool event (QEvent* ev) override;
    void paintEvent (QPaintEvent*) override;
    bool eventFilter (QObject* obj, QEvent* ev) override;


};


#endif //DSIP_SIT_PROGRESS_H
