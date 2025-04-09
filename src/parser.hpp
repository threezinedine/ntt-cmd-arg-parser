#pragma once
#include <NTTLib.hpp>

namespace NTT_NS
{
    /**
     * A comprehensive abstract utilities for handling the input arguments which the user
     *      passes from the command line to the program. This class is inspired by the the
     *      argparse library in Python with the similer simple user interface.ArgParser
     * The option -h or --help will show the description of the parser.
     *
     * @example
     * ```c++
     * #include <NTTArgParser.hpp>
     *
     * ArgParser parser("This is the description of the parser");
     *
     * parser.add_argument("-v", "--version", ArgParser::ArgType::STRING, "Show the version of the program");
     *
     * parser.is_parsed() // false if the arguments are not parsed yet
     *
     * try
     * {
     *     parser.parse_args(argc, argv);
     * }
     * catch (const std::exception &e)
     * {
     *     std::cerr << e.what() << std::endl;
     *     return EXIT_FAILURE;
     * }
     *
     * parser.is_parsed() // true if the arguments are parsed successfully
     *
     * parser.get_args<String>("--version") // get the argument value as String
     * parser.get_args<int>("-v") // get the argument value as int
     * ```
     *
     */
    class ArgParser
    {
        NTT_PRIVATE_DEF(ArgParser);

    public:
        /**
         * @param description The description of the parser, this will be shown when the user which
         *      will be shown when the user runs the program with the `-h` or `--help` argument.
         */
        ArgParser(const String &description);
        ~ArgParser();

    public:
        /**
         * Configuration method of the parser, each argument will be defined into the parser via this
         *      method, the argument can be any string and that value will be extracted as the argument
         *      value. If any key which is already defined, the error will be thrown.
         *
         * @param triggerKeys The keys which will be used to trigger the argument, this can be a single
         *      or list of keys, for example: `{"-v", "--version"}` or `{"-v"}`. With the first example,
         *      you can type both `-v` or `--version` to trigger the argument, with the second example,
         *      you can only type `-v` to trigger the argument.
         *
         * @tparam T only inside `String`, `F32`, `I32`, `bool`, other types are not supported.
         *
         * @param description The description which will be shown when user get helper from the the parser.
         *
         * @param isRequired If `true`, the parser will raise the error if the argument is not loaded
         *      from the command line, vice versa.
         *
         * @param defaultValue The default value of the argument, this will be used if the argument is not
         *      provided from the command line.
         */
        template <typename T>
        void addArgument(
            const std::vector<String> &triggerKeys,
            const String &description = NTT_STRING_EMPTY,
            bool isRequired = false,
            const T defaultValue = T());

        /**
         * Used in the main function for parsing the arguments from the command line.
         *
         * @param argc The number of arguments passed to the program.
         * @param argv The arguments passed to the program.
         *
         * @example
         * ```c++
         * int main(int argc, char **argv)
         * {
         *     ArgParser parser("This is the description of the parser");
         *     parser.addArgument("-v", ArgParser::ArgType::STRING, "Show the version of the program");
         *     parser.parse(argc, argv);
         * }
         * ```
         */
        void parse(u32 argc, char **argv);

        /**
         * Obtain the argument value as the type which is specified. If the
         *      type @tparam T is not the same as the type in defined, the
         *      error will be thrown.
         *
         * @tparam T The type of the argument.
         * @param key The key of the argument. If the key is not found, the error will be thrown.
         *
         * @return The value of the argument.
         */
        template <typename T>
        T getArgument(const String &key);

        /**
         * Renew the states of the parser result, the isParsed() will return false after calling this
         *      function.
         * All the argurment defintions will be kept.
         */
        void reset();

    public:
        /**
         * @retval true if the arguments are parsed successfully.
         * @retval false if the arguments are not parsed yet or the parsing failed.
         */
        inline bool isParsed() const { return m_isParsed; }

    private:
        bool m_isParsed = false;
    };
} // namespace NTT_NS
