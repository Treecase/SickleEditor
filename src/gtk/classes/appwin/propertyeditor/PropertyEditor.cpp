/**
 * PropertyEditor.hpp - The Sickle object property editor.
 * Copyright (C) 2024 Trevor Last
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "PropertyEditor.hpp"
#include "CellRendererProperty.hpp"


using namespace Sickle::AppWin;


static Glib::ustring generate_tooltip(
    std::shared_ptr<Sickle::Editor::EntityPropertyDefinition> const &property);
static Glib::RefPtr<Gtk::ListStore> generate_choices(
    std::shared_ptr<Sickle::Editor::EntityPropertyDefinition> const &property);


PropertyEditor::PropertyEditor()
:   Glib::ObjectBase{typeid(PropertyEditor)}
,   _prop_entity{*this, "entity", {}}
,   _store{Gtk::ListStore::create(_columns())}
,   _properties{_store}
{
    _store->set_sort_column(_columns().name, Gtk::SortType::SORT_ASCENDING);

    _renderer.choices_renderer().filter_edit =\
        [this](auto const &p, auto const &v){
            return _choices_filter_edit(p, v);
        };

    auto col = Gtk::make_managed<Gtk::TreeViewColumn>("Value", _renderer);
    col->add_attribute(_renderer.property_value(), _columns().renderer_value);
    col->add_attribute(_renderer.property_choices_model(), _columns().choices);

    _properties.append_column("Name", _columns().name);
    _properties.append_column(*col);
    _properties.set_tooltip_column(_columns().tooltip.index());

    _frame.set_label("Properties");

    // Add widgets.
    _scroll.add(_properties);
    _frame.add(_scroll);
    add(_frame);

    // Connect signals.
    property_entity().signal_changed().connect(
        sigc::mem_fun(*this, &PropertyEditor::on_entity_changed));
    _renderer.signal_changed().connect(
        sigc::mem_fun(*this, &PropertyEditor::on_value_edited));
}


void PropertyEditor::set_entity(Editor::EntityRef const &entity)
{
    property_entity().set_value(entity);
}


Sickle::Editor::EntityRef PropertyEditor::get_entity() const
{
    return property_entity().get_value();
}



void PropertyEditor::on_entity_changed()
{
    _conn_entity_properties_changed.disconnect();
    on_entity_properties_changed();
    if (auto const entity = get_entity())
    {
        _conn_entity_properties_changed = entity->signal_properties_changed()
            .connect(
                sigc::mem_fun(
                    *this,
                    &PropertyEditor::on_entity_properties_changed));
    }
}


void PropertyEditor::on_entity_properties_changed()
{
    auto const entity = get_entity();
    if (!entity)
        return;

    std::unordered_set<std::string> existing_properties{};
    std::vector<Gtk::TreeModel::iterator> iters_to_delete{};

    auto const &ec = entity->classinfo();
    _store->foreach_iter(
        [this, entity, ec, &existing_properties, &iters_to_delete](
            Gtk::TreeModel::iterator const &it) -> bool
        {
            auto const &name = it->get_value(_columns().name);
            existing_properties.insert(name);

            std::string value{};
            try {
                value = entity->get_property(name);
            }
            // Property exists in the tree, but not on the entity, so
            // remove it from the tree.
            catch (std::out_of_range const &) {
                iters_to_delete.push_back(it);
                return false;
            }

            // Property exists in the tree and on the entity. We just need
            // to update the value.
            auto const &property_definition = ec.get_property(name);
            _update_row(it, name, value, property_definition);

            return false;
        }
    );

    for (auto const &it : iters_to_delete)
        _store->erase(it);

    for (auto const &[name, value] : entity->properties())
    {
        if (existing_properties.count(name) == 0)
        {
            // Property does not exist in the tree, but does exist on the
            // entity, so add it to the tree.
            auto const &property_definition = ec.get_property(name);
            auto it = _store->append();
            _update_row(it, name, value, property_definition);
        }
    }
}


void PropertyEditor::on_value_edited(
    Glib::ustring const &path,
    Glib::ustring const &value)
{
    auto const it = _store->get_iter(path);
    auto const name = it->get_value(_columns().name);
    auto the_value = it->get_value(_columns().renderer_value);

    the_value.value = value;

    it->set_value(_columns().renderer_value, the_value);
    get_entity()->set_property(name, value);
}



PropertyEditor::Columns const &PropertyEditor::_columns()
{
    static Columns const the_columns{};
    return the_columns;
}


void PropertyEditor::_update_row(
    Gtk::TreeModel::iterator const &it,
    Glib::ustring const &name,
    std::string const &value,
    std::shared_ptr<
        Sickle::Editor::EntityPropertyDefinition> const &property_definition)
{
    it->set_value<Glib::ustring>(_columns().name, name);
    it->set_value(
        _columns().renderer_value,
        CellRendererProperty::ValueType{
            value,
            property_definition});
    it->set_value(
        _columns().tooltip,
        generate_tooltip(property_definition));
    it->set_value(
        _columns().choices,
        generate_choices(property_definition));
}


Glib::ustring PropertyEditor::_choices_filter_edit(
    Glib::ustring const &path,
    Glib::ustring const &choice) const
{
    auto const it = _store->get_iter(path);
    auto const choices = it->get_value(_columns().choices);
    Glib::ustring filtered = choice;
    choices->foreach_iter(
        [this, choice, &filtered](Gtk::TreeModel::iterator const &it) -> bool
        {
            auto const desc = it->get_value(
                CellRendererProperty::ChoicesRenderer::columns().desc);
            if (desc == choice)
            {
                filtered = std::to_string(
                    it->get_value(
                        CellRendererProperty::ChoicesRenderer::columns().idx));
                return true;
            }
            else
                return false;
        });
    return filtered;
}



static Glib::ustring generate_tooltip(
    std::shared_ptr<Sickle::Editor::EntityPropertyDefinition> const &property)
{
    auto const flags =\
        std::dynamic_pointer_cast<
            Sickle::Editor::EntityPropertyDefinitionFlags>(property);
    if (!flags)
        return "";

    Glib::ustring text{};
    for (int i = 0; i < 32; ++i)
    {
        auto const desc = flags->get_description(i);
        if (!desc.empty())
            text += std::to_string(i) + ": " + desc + "\n";
    }
    if (!text.empty())
        text.erase(text.size() - 1);
    return text;
}


static Glib::RefPtr<Gtk::ListStore> generate_choices(
    std::shared_ptr<Sickle::Editor::EntityPropertyDefinition> const &property)
{
    Glib::RefPtr<Gtk::ListStore> output{nullptr};
    auto const choices =\
        std::dynamic_pointer_cast<
            Sickle::Editor::EntityPropertyDefinitionChoices>(property);
    if (!choices)
        return output;

    auto const &columns = CellRendererProperty::ChoicesRenderer::columns();
    output = Gtk::ListStore::create(columns);
    for (auto const &kv : choices->choices())
    {
        auto it = output->append();
        it->set_value(columns.idx, kv.first);
        it->set_value(columns.desc, Glib::ustring{kv.second});
    }
    return output;
}
