// These are the method of the CharybdeFS thrift client object
//
// For more info on how to use thrift with python
// look at: https://thrift.apache.org/tutorial/py
//

service server {

    // Used to get the list of availables systems calls
    list<string> get_methods(),

    // Used to clear all faults sources
    void clear_all_faults(),

    // Used to clear a specific method fault
    void clear_fault(string method),

    // Set fault on a specific list of methods
    void set_fault(list<string> methods,    // the list of methods to operate on
                   bool random,             // Must we return random errno
                   list<i32> err_nos,       // A list of specific errnos to select from
                   i32 probability,         // Fault probability over 100 000
                   string regexp,           // A regexp matching a victim file
                   bool kill_caller,        // Kill -9 the caller process
                   i32 delay_us,            // Delay to inject in the fs calls
                   bool auto_delay),        // Not implemented yet: Will be used to simulate SSDs latencies

    // Works like set_fault but applies the fault to all methods
    void set_all_fault(bool random,
                       list<i32> err_nos,
                       i32 probability,
                       string regexp,
                       bool kill_caller,
                       i32 delay_us,
                       bool auto_delay),
}
