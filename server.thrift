service server {

    list<string> get_methods(), 

    void clear_all_faults(),

    void clear_fault(string method),

    void set_fault(list<string> methods, bool random, i32 err_no, i32 probability, string regexp, bool kill_caller, i32 delay_us, bool auto_delay),

    void set_all_fault(bool random, i32 err_no, i32 probability, string regexp, bool kill_caller, i32 delay_us, bool auto_delay),
}
