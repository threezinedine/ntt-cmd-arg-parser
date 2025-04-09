#include "parser.hpp"
#include <exception>
#include <stdexcept>
#include <cstring>
#include "memory.hpp"

// The number of arguments inside this module usually will not exceed 10 so that using linear search
//      is acceptable.

#define NTT_ARGUMENT_INVALID_INDEX -1

namespace NTT_NS
{
    /**
     * The list of availabel types which the arg can be passed into the
     *      argparser, this list can be modified later.
     */
    enum ArgParserType
    {
        STRING,
        I32,
        F32,
        BOOL,
    };

    /**
     * Store the number value of the argument.
     */
    union ArgumentValue
    {
        i32 i32Value;
        f32 f32Value;
        bool boolValue;

        ArgumentValue() : i32Value(0) {}
    };

    /**
     * Store all needed information of the argument (definition
     *      information, value, etc.).
     */
    struct ArgumentData
    {
        std::vector<String> triggerKeys;
        String description;
        String stringValue;
        String defaultStringValue;
        ArgParserType type;
        ArgumentValue value;
        ArgumentValue defaultValue;
        bool isRequired;
        bool provided;
    };

    class ArgParser::ArgParserPrivate
    {
    public:
        String description;
        std::vector<Scope<ArgumentData>> arguments;
        std::vector<u32> requiredArgumentIndexes;

        i64 searchByKey(const String &key)
        {
            for (u32 i = 0; i < arguments.size(); i++)
            {
                Scope<ArgumentData> &argument = arguments[i];
                for (const String &triggerKey : argument->triggerKeys)
                {
                    if (triggerKey == key)
                    {
                        return i;
                    }
                }
            }

            return NTT_ARGUMENT_INVALID_INDEX;
        }
    };

    ArgParser::ArgParser(const String &description)
    {
        impl = CreateScope<ArgParserPrivate>();
        impl->description = description;
    }

    ArgParser::~ArgParser() {}

    /**
     * override for printing method of with the argument value.
     */
    template <>
    String format(const String &formatMsg, const ArgumentValue &value)
    {
        const String data = format("<int={}, float={}, bool={}>",
                                   value.i32Value,
                                   value.f32Value,
                                   value.boolValue);
        return format(formatMsg, data);
    }

    /**
     * override for printing method of with the argument type.
     */
    template <>
    String format(const String &formatMsg, const ArgParserType &value)
    {
        String typeStr = NTT_STRING_EMPTY;
        switch (value)
        {
        case ArgParserType::STRING:
            typeStr = "STRING";
            break;
        case ArgParserType::I32:
            typeStr = "I32";
            break;
        case ArgParserType::F32:
            typeStr = "F32";
            break;
        case ArgParserType::BOOL:
            typeStr = "BOOL";
            break;
        default:
            typeStr = "UNKNOWN";
            break;
        }

        return format(formatMsg, typeStr);
    }

    /**
     * override for printing method of with the argument data.
     */
    template <>
    String format(const String &formatMsg, const ArgumentData &value)
    {
        const String data = format(R"({
    keys: {}
    description: {}
    type: {}
    value: {}
    isRequired: {}
    stringValue: {}
    provided: {}
})",
                                   value.triggerKeys,
                                   value.description,
                                   value.type,
                                   value.value,
                                   value.isRequired,
                                   value.stringValue,
                                   value.provided);
        return format(formatMsg, data);
    }

    static i32 safeStoi(const String &str, i32 defaultValue)
    {
        try
        {
            return std::stoi(str);
        }
        catch (const std::exception &e)
        {
            return defaultValue;
        }
    }

    static f32 safeStof(const String &str, f32 defaultValue)
    {
        try
        {
            return std::stof(str);
        }
        catch (const std::exception &e)
        {
            return defaultValue;
        }
    }

#define NTT_ARGUMENT_ADD_ARGUMENT_DEF(typeName, argParserType, valueState)   \
    template <>                                                              \
    void ArgParser::addArgument<typeName>(                                   \
        const std::vector<String> &triggerKeys,                              \
        const String &description,                                           \
        bool isRequired,                                                     \
        const typeName defaultValue)                                         \
    {                                                                        \
        Scope<ArgumentData> argument = CreateScope<ArgumentData>();          \
        argument->triggerKeys = triggerKeys;                                 \
        argument->type = argParserType;                                      \
        argument->description = description;                                 \
        argument->isRequired = isRequired;                                   \
        argument->value = ArgumentValue();                                   \
        valueState;                                                          \
        argument->provided = false;                                          \
                                                                             \
        if (isRequired)                                                      \
        {                                                                    \
            impl->requiredArgumentIndexes.push_back(impl->arguments.size()); \
        }                                                                    \
                                                                             \
        impl->arguments.push_back(std::move(argument));                      \
    }

    NTT_ARGUMENT_ADD_ARGUMENT_DEF(
        String, ArgParserType::STRING,
        argument->stringValue = defaultValue;
        argument->defaultStringValue = defaultValue;
        argument->value.boolValue = false);
    NTT_ARGUMENT_ADD_ARGUMENT_DEF(
        i32, ArgParserType::I32,
        argument->value.i32Value = defaultValue;
        argument->defaultValue.i32Value = defaultValue;
        argument->stringValue = NTT_STRING_EMPTY);
    NTT_ARGUMENT_ADD_ARGUMENT_DEF(
        f32, ArgParserType::F32,
        argument->value.f32Value = defaultValue;
        argument->defaultValue.f32Value = defaultValue;
        argument->stringValue = NTT_STRING_EMPTY);
    NTT_ARGUMENT_ADD_ARGUMENT_DEF(
        bool, ArgParserType::BOOL,
        argument->value.boolValue = defaultValue;
        argument->defaultValue.boolValue = defaultValue;
        argument->stringValue = NTT_STRING_EMPTY);

    void ArgParser::parse(u32 argc, char **argv)
    {
        reset();

        i64 currentIndex = NTT_ARGUMENT_INVALID_INDEX;

        // The first argument is the program name, so we start from the second argument.
        for (u32 i = 1; i < argc; i++)
        {
            String arg = argv[i];
            currentIndex = impl->searchByKey(arg);

            if (currentIndex == NTT_ARGUMENT_INVALID_INDEX)
            {
                throw std::invalid_argument(format("The key {} is not found", arg).c_str());
            }

            if (currentIndex != NTT_ARGUMENT_INVALID_INDEX)
            {
                Scope<ArgumentData> &argument = impl->arguments[currentIndex];

                if (argument->type == ArgParserType::STRING)
                {
                    if (i + 1 >= argc)
                    {
                        throw std::invalid_argument(
                            format("The string argument {} is not followed by a value",
                                   argument->triggerKeys)
                                .c_str());
                    }

                    argument->stringValue = argv[i + 1];
                    argument->provided = true;
                    i++;
                }
                else if (argument->type == ArgParserType::I32)
                {
                    if (i + 1 >= argc)
                    {
                        throw std::invalid_argument(
                            format("The i32 argument {} is not followed by a value",
                                   argument->triggerKeys)
                                .c_str());
                    }

                    argument->value.i32Value = safeStoi(argv[i + 1], argument->defaultValue.i32Value);
                    argument->provided = true;
                    i++;
                }
                else if (argument->type == ArgParserType::F32)
                {
                    if (i + 1 >= argc)
                    {
                        throw std::invalid_argument(
                            format("The f32 argument {} is not followed by a value",
                                   argument->triggerKeys)
                                .c_str());
                    }

                    argument->value.f32Value = safeStof(argv[i + 1], argument->defaultValue.f32Value);
                    argument->provided = true;
                    i++;
                }
                else if (argument->type == ArgParserType::BOOL)
                {
                    if (i + 1 >= argc)
                    {
                        argument->value.boolValue = true;
                        argument->provided = true;
                        continue;
                    }

                    if (strcmp(argv[i + 1], "true") == 0 || strcmp(argv[i + 1], "false") == 0)
                    {
                        argument->value.boolValue = strcmp(argv[i + 1], "true") == 0;
                        argument->provided = true;
                        i++;
                    }
                }
                else
                {
                    throw std::invalid_argument("The type is not supported");
                }
            }
        }

        for (u32 i = 0; i < impl->requiredArgumentIndexes.size(); i++)
        {
            u32 index = impl->requiredArgumentIndexes[i];
            Scope<ArgumentData> &argument = impl->arguments[index];
            if (!argument->provided)
            {
                throw std::invalid_argument(
                    format("The required argument {} is not provided", argument->triggerKeys).c_str());
            }
        }

        m_isParsed = true;
    }

    void ArgParser::reset()
    {
        for (Scope<ArgumentData> &argument : impl->arguments)
        {
            switch (argument->type)
            {
            case ArgParserType::STRING:
                argument->stringValue = argument->defaultStringValue;
                break;
            case ArgParserType::I32:
                argument->value.i32Value = argument->defaultValue.i32Value;
                break;
            case ArgParserType::F32:
                argument->value.f32Value = argument->defaultValue.f32Value;
                break;
            case ArgParserType::BOOL:
                argument->value.boolValue = argument->defaultValue.boolValue;
                break;
            default:
                break;
            }
        }
    }

#define NTT_ARGUMENT_GET_VALUE_DEF(typeName, argParserType, getStatement)                                         \
    template <>                                                                                                   \
    typeName ArgParser::getArgument<typeName>(const String &key)                                                  \
    {                                                                                                             \
        i64 index = impl->searchByKey(key);                                                                       \
        if (index == NTT_ARGUMENT_INVALID_INDEX)                                                                  \
        {                                                                                                         \
            throw std::invalid_argument(format("The key {} is not found", key).c_str());                          \
        }                                                                                                         \
                                                                                                                  \
        Scope<ArgumentData> &argument = impl->arguments[index];                                                   \
        if (argument->type != argParserType)                                                                      \
        {                                                                                                         \
            throw std::invalid_argument(format("The key {} is not a " #typeName, argument->triggerKeys).c_str()); \
        }                                                                                                         \
                                                                                                                  \
        return getStatement;                                                                                      \
    }

    NTT_ARGUMENT_GET_VALUE_DEF(String, ArgParserType::STRING, argument->stringValue);
    NTT_ARGUMENT_GET_VALUE_DEF(i32, ArgParserType::I32, argument->value.i32Value);
    NTT_ARGUMENT_GET_VALUE_DEF(f32, ArgParserType::F32, argument->value.f32Value);
    NTT_ARGUMENT_GET_VALUE_DEF(bool, ArgParserType::BOOL, argument->value.boolValue);
} // namespace NTT_NS
