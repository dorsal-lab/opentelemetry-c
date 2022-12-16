#ifndef CONTEXT_H
#define CONTEXT_H

/**
 * @brief Combine message and context
 *
 * The generated string has this format : <context-length> <context> <message>
 * The function allocated memory. The caller is responsible of freeing that
 * memory.
 *
 * @param message The message
 * @param context The context
 * @return char* The message and context combined
 */
char *add_context_to_message(const char *message, const char *context);

/**
 * @brief Split message and context
 *
 * @param message_and_context The message and context combined.
 * It must have this format this format :
 * <context-length> <context>
 * @param message A pointer to message string. The function allocated memory
 * that must be freed by the caller.
 * @param context A pointer to context string. The function allocated memory
 * that must be freed by the caller.
 */
void split_message_and_context(const char *message_and_context, char **message,
                               char **context);

#endif // !CONTEXT_H