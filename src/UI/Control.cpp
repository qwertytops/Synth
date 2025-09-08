#include "UI/Control.hpp"


QMenu* Control::inputsMenu()
{
    QMenu* menu = new QMenu(this);

    // add mainOut to menu
    QAction* action = menu->addAction("Main Out");
    connect(action, &QAction::triggered, this, [this]() {
        NoteInput* selected = this->parentComponent->synth->mainOut;
        Connection* conn = new Connection(this->parentComponent, selected);
        this->parentComponent->outgoingConnections.push_back(conn);
    });
    
    // add all other components' inputs
    for (auto& comp : this->parentComponent->synth->components) {
        QMenu* submenu = menu->addMenu(QString::fromStdString(comp->name) + QString(" %1").arg(comp->id));
        for (int i = 0; i < comp->inputs.size(); ++i) {
            QAction* action = submenu->addAction(QString::fromStdString(comp->inputs[i]->name));
            action->setData(i); // store index of NoteInput
            connect(action, &QAction::triggered, this, [this, comp, i]() {
                NoteInput* selected = comp->inputs[i];
                Connection* conn = new Connection(this->parentComponent, selected);

                cout << "new connection from " << conn->source->name << conn->source->id << " to " << conn->destination->name << endl;

                this->parentComponent->outgoingConnections.push_back(conn);
                selected->parent->incomingConnections.push_back(conn);
                this->parentComponent->synth->establishProcessingOrder();
            });
        }
    }

    return menu;
}