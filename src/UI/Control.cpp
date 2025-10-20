#include "UI/Control.hpp"


QMenu* Control::availableInputsMenu()
{
    QMenu* menu = new QMenu(this);

    // add mainOut to menu
    QAction* action = menu->addAction("Main Out");
    connect(action, &QAction::triggered, this, [this]() {
        Input* selected = this->parentComponent->synth->mainOut;
        Connection* conn = new Connection(this->parentComponent, selected);
        this->parentComponent->outgoingConnections.push_back(conn);
    });
    
    // add all other components' inputs
    for (auto& comp : this->parentComponent->synth->components) {
        QMenu* submenu = menu->addMenu(QString::fromStdString(comp->name) + QString(" %1").arg(comp->id));
        for (int i = 0; i < comp->inputs.size(); ++i) {
            QAction* action = submenu->addAction(QString::fromStdString(comp->inputs[i]->name));
            action->setData(i); // store index of Input
            connect(action, &QAction::triggered, this, [this, comp, i]() {
                Input* selected = comp->inputs[i];
                Connection* conn = new Connection(this->parentComponent, selected);

                this->parentComponent->outgoingConnections.push_back(conn);
                selected->parent->incomingConnections.push_back(conn);
                this->parentComponent->synth->establishProcessingOrder();
            });
        }
    }

    return menu;
}

QMenu* Control::existingConnectionsMenu(SynthComponent* component) {
    QMenu* menu = new QMenu(this);

    for (int i = 0; i < component->outgoingConnections.size(); i++) {
        cout << "herex" << endl;
        Connection *conn = component->outgoingConnections.at(i);
        QAction* action = menu->addAction(QString::fromStdString(conn->destination->name));
        action->setData(i);
        connect(action, &QAction::triggered, this, [this, component, conn, i]() {
            auto destConns = conn->destination->parent->incomingConnections;
            vector<Connection*>::iterator position = std::find(destConns.begin(), destConns.end(), conn);
            destConns.erase(position);

            auto srcConns = component->outgoingConnections;
            position = std::find(srcConns.begin(), srcConns.end(), conn);
            srcConns.erase(position);
            
            free(conn);
        });
    }

    return menu;
}