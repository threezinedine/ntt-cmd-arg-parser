#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <NTTArgParser.hpp>

using namespace NTT_NS;

class ArgParserTest : public ::testing::Test
{
protected:
    ArgParser parser{"This is the description of the parser"};
    String test1 = "program -v 1.0.0 --col 8 -r 9.5 --use-color";
    String test2 = "program -v 1.2.0 --col -3 -r 2.12";
    u32 argCount = 0;
    char **argValues = nullptr;

    void SetUp() override
    {
        LoadArgument(test1);
    }

    void LoadArgument(const String &test)
    {
        DeleteArgument();

        std::vector<String> args = test.split(" ", {"\"", "\""});
        argCount = args.size();
        argValues = new char *[argCount];
        for (u32 i = 0; i < argCount; i++)
        {
            String arg = args[i];

            if (arg.startsWith("\""))
            {
                arg = arg.substring(1, arg.length() - 2);
            }

            argValues[i] = new char[arg.length() + 1];
            strcpy(argValues[i], arg.c_str());
            argValues[i][arg.length()] = '\0';
        }
    }

    void DefineArgument()
    {
        parser.addArgument<String>(
            {"-v", "--version"},
            "Show the version of the program",
            false, "1.0.0");
        parser.addArgument<i32>(
            {"-c", "--col"},
            "Show the color of the program");
        parser.addArgument<f32>(
            {"-r", "--radius"},
            "Show the radius of the program",
            true, 1.0f);
        parser.addArgument<bool>(
            {"--use-color"},
            "Show the color of the program");
    }

    void DeleteArgument()
    {
        if (argValues == nullptr)
            return;

        for (u32 i = 0; i < argCount; i++)
        {
            if (argValues[i] == nullptr)
                continue;

            delete[] argValues[i];
            argValues[i] = nullptr;
        }
        delete[] argValues;
        argValues = nullptr;
        argCount = 0;
    }

    void TearDown() override
    {
        DeleteArgument();
    }
};

TEST_F(ArgParserTest, AtTheBeginningTheParserIsNotParsed)
{
    EXPECT_EQ(parser.isParsed(), false);
}

TEST_F(ArgParserTest, AddExampleArgument)
{
    EXPECT_NO_THROW(
        parser.addArgument<String>(
            {"-v", "--version"},
            "Show the version of the program"));
}

TEST_F(ArgParserTest, ParseTheExampleArgument)
{
    DefineArgument();
    parser.parse(argCount, argValues);

    EXPECT_THAT(parser.getArgument<String>("-v"), ::testing::StrEq("1.0.0"));
    EXPECT_THAT(parser.getArgument<i32>("-c"), ::testing::Eq(8));
    EXPECT_THAT(parser.getArgument<f32>("-r"), ::testing::Eq(9.5f));
    EXPECT_THAT(parser.getArgument<bool>("--use-color"), ::testing::Eq(true));

    LoadArgument(test2);
    parser.parse(argCount, argValues);

    EXPECT_THAT(parser.getArgument<String>("-v"), ::testing::StrEq("1.2.0"));
    EXPECT_THAT(parser.getArgument<i32>("-c"), ::testing::Eq(-3));
    EXPECT_THAT(parser.getArgument<f32>("-r"), ::testing::Eq(2.12f));
    EXPECT_THAT(parser.getArgument<bool>("--use-color"), ::testing::Eq(false));
}

TEST_F(ArgParserTest, GetArgumentValueWithWrongType)
{
    DefineArgument();
    parser.parse(argCount, argValues);

    EXPECT_THROW(parser.getArgument<String>("-c"), std::invalid_argument);
    EXPECT_THROW(parser.getArgument<f32>("-c"), std::invalid_argument);
    EXPECT_THROW(parser.getArgument<bool>("-c"), std::invalid_argument);

    EXPECT_THROW(parser.getArgument<String>("-r"), std::invalid_argument);
    EXPECT_THROW(parser.getArgument<i32>("-r"), std::invalid_argument);
    EXPECT_THROW(parser.getArgument<bool>("-r"), std::invalid_argument);

    EXPECT_THROW(parser.getArgument<String>("--use-color"), std::invalid_argument);
    EXPECT_THROW(parser.getArgument<i32>("--use-color"), std::invalid_argument);
    EXPECT_THROW(parser.getArgument<f32>("--use-color"), std::invalid_argument);
}

TEST_F(ArgParserTest, GetArgumentValueWithWrongKey)
{
    DefineArgument();
    parser.parse(argCount, argValues);

    EXPECT_THROW(parser.getArgument<String>("-t"), std::invalid_argument);
    EXPECT_THROW(parser.getArgument<i32>("-t"), std::invalid_argument);
    EXPECT_THROW(parser.getArgument<f32>("-t"), std::invalid_argument);
    EXPECT_THROW(parser.getArgument<bool>("-t"), std::invalid_argument);
}

TEST_F(ArgParserTest, ParseWithRequiredArgumentButNotProvided)
{
    DefineArgument();

    const String notProvided = "program -v 1.0.0";
    LoadArgument(notProvided);

    EXPECT_THROW(parser.parse(argCount, argValues), std::invalid_argument);
    EXPECT_EQ(parser.isParsed(), false);
}

TEST_F(ArgParserTest, ParseWithMissingNonRequiredArgument)
{
    DefineArgument();

    const String notProvided = "program -r 1.0";
    LoadArgument(notProvided);

    EXPECT_NO_THROW(parser.parse(argCount, argValues));
    EXPECT_EQ(parser.isParsed(), true);
    EXPECT_EQ(parser.getArgument<f32>("-r"), 1.0f);
    EXPECT_EQ(parser.getArgument<String>("-v"), "1.0.0");
}

TEST_F(ArgParserTest, InputInvalidArgumentType)
{
    DefineArgument();

    const String invalid = "program -v 1.0.0 -c \"Testing\" -r \"Hello World\" --use-color";
    LoadArgument(invalid);

    EXPECT_NO_THROW(parser.parse(argCount, argValues));
    EXPECT_EQ(parser.getArgument<f32>("-r"), 1.0f);
    EXPECT_EQ(parser.getArgument<i32>("-c"), 0);
}
