# [Snohandle](http://wiki.znc.in/Snohandle)

Snohandle is a ZNC module that reinterprets notices addressed to neither our nick or a channel we know. In practice these are the server notices. The module causes all other modules to receive notifications about these notices as if they were private notices. This includes, for example, the log module.

This module has no commands or configuration.
