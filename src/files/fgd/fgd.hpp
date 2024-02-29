/**
 * fgd.hpp - Load .fgd files.
 * Copyright (C) 2022 Trevor Last
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

#ifndef SE_FGD_HPP
#define SE_FGD_HPP

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <sstream>
#include <vector>


namespace FGD
{
    struct Property
    {
        std::string name;
        virtual std::string type() const=0;
        virtual std::string rest() const {return "";}
        Property(std::string const &name): name{name} {}
        virtual ~Property() {};
    };
    struct DescriptionProperty : public Property
    {
        std::optional<std::string> description;
        virtual std::string rest() const override
        {
            if (description.has_value())
                return " : \"" + description.value() + '"';
            return "";
        }
        DescriptionProperty(
            std::string const &name,
            std::optional<std::string> const &description)
        : Property{name}, description{description}
        {}
        virtual ~DescriptionProperty() {};
    };
    struct IntegerProperty : public DescriptionProperty
    {
        std::optional<int> defaultvalue;
        virtual std::string type() const override {return "integer";}
        virtual std::string rest() const override
        {
            std::stringstream ss{};
            ss << DescriptionProperty::rest();
            if (defaultvalue.has_value())
                ss << " : " << defaultvalue.value();
            return ss.str();
        }
        IntegerProperty(
            std::string const &name, std::string const &description,
            std::optional<int> const &defaultvalue)
        : DescriptionProperty{name, description}, defaultvalue{defaultvalue}
        {}
        virtual ~IntegerProperty() {};
    };
    struct StringProperty : public DescriptionProperty
    {
        std::optional<std::string> defaultvalue;
        virtual std::string type() const override {return "string";}
        virtual std::string rest() const override
        {
            std::stringstream ss{};
            ss << DescriptionProperty::rest();
            if (defaultvalue.has_value())
                ss << " : \"" << defaultvalue.value() << '"';
            return ss.str();
        }
        StringProperty(
            std::string const &name, std::string const &description,
            std::optional<std::string> const &defaultvalue)
        : DescriptionProperty{name, description}, defaultvalue{defaultvalue}
        {}
        virtual ~StringProperty() {};
    };
    struct ChoiceProperty : public DescriptionProperty
    {
        std::optional<int> defaultvalue;
        std::map<int, std::string> choices;
        virtual std::string type() const override {return "choices";}
        virtual std::string rest() const override
        {
            std::stringstream ss{};
            ss << DescriptionProperty::rest();
            if (defaultvalue.has_value())
                ss << " : " << defaultvalue.value();
            ss << " =\n\t[";
            for (auto const &choice : choices)
                ss << "\n\t\t" << choice.first
                    << ": \"" << choice.second << '"';
            ss << "\n\t]";
            return ss.str();
        }
        ChoiceProperty(
            std::string const &name,
            std::optional<std::string> const &description,
            std::optional<int> defaultvalue,
            std::map<int, std::string> const &choices)
        :   DescriptionProperty{name, description}
        , defaultvalue{defaultvalue}, choices{choices}
        {}
        virtual ~ChoiceProperty() {};
    };
    struct FlagProperty : public Property
    {
        struct Flag {std::string description; int start_value;};
        std::map<int, Flag> flags;
        virtual std::string type() const override {return "flags";}
        virtual std::string rest() const override
        {
            std::stringstream ss{};
            ss << " =\n\t[";
            for (auto const &flag : flags)
                ss << "\n\t\t" << flag.first
                    << ": \"" << flag.second.description << '"'
                    << " : " << flag.second.start_value;
            ss << "\n\t]";
            return ss.str();
        }
        FlagProperty(std::string const &name, std::map<int, Flag> const &flags)
        : Property{name}, flags{flags}
        {/* TODO: flag.first must be a power-of-2 */}
        virtual ~FlagProperty() {}
    };
    struct TargetSourceProperty : public DescriptionProperty
    {
        virtual std::string type() const override {return "target_source";}
        using DescriptionProperty::DescriptionProperty;
        virtual ~TargetSourceProperty() {}
    };
    struct TargetDestinationProperty : public DescriptionProperty
    {
        virtual std::string type() const override {return "target_destination";}
        using DescriptionProperty::DescriptionProperty;
        virtual ~TargetDestinationProperty() {}
    };
    struct Color255Property : public DescriptionProperty
    {
        std::string value;
        virtual std::string type() const override {return "color255";}
        virtual std::string rest() const override
        {
            std::stringstream ss{};
            ss << DescriptionProperty::rest() << " : \"" << value << '"';
            return ss.str();
        }
        Color255Property(
            std::string const &name, std::string const &description,
            std::string const &value)
        : DescriptionProperty{name, description}, value{value}
        {}
        virtual ~Color255Property() {}
    };
    struct StudioProperty : public StringProperty
    {
        virtual std::string type() const override {return "studio";}
        using StringProperty::StringProperty;
        virtual ~StudioProperty() {}
    };
    struct SpriteProperty : public StringProperty
    {
        virtual std::string type() const override {return "sprite";}
        using StringProperty::StringProperty;
        virtual ~SpriteProperty() {}
    };
    struct SoundProperty : public StringProperty
    {
        virtual std::string type() const override {return "sound";}
        using StringProperty::StringProperty;
        virtual ~SoundProperty() {}
    };
    struct DecalProperty : public DescriptionProperty
    {
        virtual std::string type() const override {return "decal";}
        using DescriptionProperty::DescriptionProperty;
        virtual ~DecalProperty() {}
    };

    struct Attribute
    {
        virtual std::string name() const=0;
        virtual std::string inner() const {return "";}
        virtual ~Attribute() {}
    };
    struct SizeAttribute : public Attribute
    {
        std::tuple<int, int, int> bbox1;
        std::optional<std::tuple<int, int, int>> bbox2;
        virtual std::string name() const override {return "size";}
        virtual std::string inner() const override
        {
            std::stringstream ss{};
            ss << std::get<0>(bbox1);
            ss << ' ' << std::get<1>(bbox1);
            ss << ' ' << std::get<2>(bbox1);
            if (bbox2.has_value())
            {
                ss << ", " << std::get<0>(bbox2.value());
                ss << ' ' << std::get<1>(bbox2.value());
                ss << ' ' << std::get<2>(bbox2.value());
            }
            return ss.str();
        }
        SizeAttribute(int a, int b, int c): bbox1{a, b, c}, bbox2{} {}
        SizeAttribute(int a, int b, int c, int x, int y, int z)
        :   bbox1{a, b, c}
        ,   bbox2{{x, y, z}}
        {}
        virtual ~SizeAttribute() {}
    };
    struct ColorAttribute : public Attribute
    {
        std::tuple<int, int, int> rgb;
        virtual std::string name() const override {return "color";}
        virtual std::string inner() const override
        {
            std::stringstream ss{};
            ss << std::get<0>(rgb);
            ss << ' ' << std::get<1>(rgb);
            ss << ' ' << std::get<2>(rgb);
            return ss.str();
        }
        ColorAttribute(int r, int g, int b): rgb{r, g, b} {}
        virtual ~ColorAttribute() {}
    };
    struct BaseAttribute : public Attribute
    {
        std::vector<std::string> baseclasses;
        virtual std::string name() const override {return "base";}
        virtual std::string inner() const override
        {
            std::stringstream ss{};
            auto sep = "";
            for (auto const &baseclass : baseclasses)
            {
                ss << sep << baseclass;
                sep = ", ";
            }
            return ss.str();
        }
        BaseAttribute(std::vector<std::string> const &baseclasses)
        :   baseclasses{baseclasses}
        {}
        virtual ~BaseAttribute() {}
    };
    struct IconSpriteAttribute : public Attribute
    {
        std::string iconpath;
        virtual std::string name() const override {return "iconsprite";}
        virtual std::string inner() const override
        {return '"' + iconpath + '"';}
        IconSpriteAttribute(std::string const &iconpath): iconpath{iconpath} {}
        virtual ~IconSpriteAttribute() {}
    };
    struct SpriteAttribute : public Attribute
    {
        virtual std::string name() const override {return "sprite";}
        virtual ~SpriteAttribute() {}
    };
    struct DecalAttribute : public Attribute
    {
        virtual std::string name() const override {return "decal";}
        virtual ~DecalAttribute() {}
    };
    struct StudioAttribute : public Attribute
    {
        std::string path;
        virtual std::string name() const override {return "studio";}
        virtual std::string inner() const override
        {return '"' + path + '"';}
        StudioAttribute(std::string const &path): path{path} {}
        virtual ~StudioAttribute() {}
    };

    struct Class
    {
        std::vector<std::shared_ptr<Attribute>> attributes;
        std::string name;
        std::optional<std::string> description;
        std::vector<std::shared_ptr<Property>> properties;
        virtual std::string type() const=0;
        Class(
            std::vector<std::shared_ptr<Attribute>> const &attributes,
            std::string const &name,
            std::optional<std::string> const &description,
            std::vector<std::shared_ptr<Property>> const &properties)
        : attributes{attributes}, name{name}, description{description}
        , properties{properties}
        {}
        virtual ~Class() {}
    };
    struct BaseClass : public Class
    {
        virtual std::string type() const override {return "BaseClass";}
        using Class::Class;
        virtual ~BaseClass() {}
    };
    struct SolidClass : public Class
    {
        virtual std::string type() const override {return "SolidClass";}
        using Class::Class;
        virtual ~SolidClass() {}
    };
    struct PointClass : public Class
    {
        virtual std::string type() const override {return "PointClass";}
        using Class::Class;
        virtual ~PointClass() {}
    };

    struct GameDef
    {
        std::vector<std::shared_ptr<Class>> classes;
    };


    GameDef from_file(std::string const &path);

    std::ostream &operator<<(std::ostream &os, GameDef const &fgd);
    std::ostream &operator<<(std::ostream &os, Class const &cls);
    std::ostream &operator<<(std::ostream &os, Attribute const &attr);
    std::ostream &operator<<(std::ostream &os, Property const &prop);
}

#endif
