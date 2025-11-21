#include "UI/Control.hpp"
#include "UI/EmptySlot.hpp"

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

void Control::deleteComponent() {
    Synth* synth = parentComponent->synth;
    // remove module from synth
    synth->removeComponent(parentComponent);

    // replace ui with empty slot
    replaceGridItem(synth);
}

void Control::replaceGridItem(Synth* synth) {
    // Get the grid layout
    auto* grid = qobject_cast<QGridLayout*>(this->parentWidget()->layout());
    if (!grid)
        return;

    // Find the current position of `this` in the grid
    int row = -1, col = -1, rowSpan = 1, colSpan = 1;
    bool found = false;
    for (int r = 0; r < grid->rowCount() && !found; ++r) {
        for (int c = 0; c < grid->columnCount(); ++c) {
            QLayoutItem* item = grid->itemAtPosition(r, c);
            if (item && item->widget() == this) {
                row = r;
                col = c;
                found = true;
                break;
            }
        }
    }

    if (!found)
        return;

    // Remove and delete the old widget
    QLayoutItem* item = grid->takeAt(grid->indexOf(this));
    delete item;             // removes layout item
    this->hide();
    this->deleteLater();     // schedule deletion

    // Place the new widget in the same grid cell
    grid->addWidget(new EmptySlot(synth), row, col, rowSpan, colSpan);
}