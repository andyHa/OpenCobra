//
// Provices a simple logging facility
//

#ifndef RASPICSP_LOGGER_H
#define RASPICSP_LOGGER_H

/*
 * Provides simple logging methods
 */
class Logger {
private:
    static int tracing;
public:
    /*
     * Logs a printf style message to stdout
     */
    static void log(const char *category, const char *format, ...);

    /*
     * Logs a printf style message to stdout if tracing is enabled
     */
    static void trace(const char *category, const char *format, ...);

    /*
     * Enables the output of trace messages
     */
    static void enable_tracing();

    /**
     * Determines if tracing is enabled
     */
    static int is_tracing();
};

#endif //RASPICSP_LOGGER_H
