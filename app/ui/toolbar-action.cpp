#include "toolbar-action.h"

ToolbarAction::ToolbarAction(QWidget *parent)
    : QWidget (parent)
{
    setFixedSize(30, 30);

    setStyleSheet("background-color:black");
}
