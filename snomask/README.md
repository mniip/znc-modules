# [Snomask](http://wiki.znc.in/Snomask)

Snomask is a ZNC module that filters server notices based on the client identifier. One can configure what patterns correspond to what snomask letters, and then configure snomasks per client identifiers.
Use `AddClient` and `DelClient` to add and remove a client identifier. `ListClients` lists client identifiers and their respective snomasks. `Snomask +-<snomasks>` adds or removes letters from the snomask.

Note that this module only does filtering. You will not receive any server notices you haven't requested via the actual server-side snomask.

Snomask definitions can be configured via the webadmin, or added, removed, and shuffled around with `AddDef`, `DelDef`, `SwapDef` respectively. To list definitions use `ListDefs`. `DefaultDefs` will restore the definitions list to the default (currently includes charybdis snotices excluding SNO_GENERAL).
