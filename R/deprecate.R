# We want to warn users that the new function naming should be used instead of
# the old one, so we issue a message when users call a deprecated function.
# However, this may be too annoying, so we print the message only once for
# each function.
# The idea is that we use an environment to record whether the user has been
# warned on a function.

.warned_list = new.env()

deprecate_message_once = function(old_fun, new_fun)
{
    msg = sprintf("'%s' is now renamed to '%s'
The old version still works, but consider using the new function in future code",
                  old_fun, new_fun)
    
    warned = exists(old_fun, envir = .warned_list)
    if(!warned)
    {
        message(msg)
        assign(old_fun, TRUE, envir = .warned_list)
    }
}
