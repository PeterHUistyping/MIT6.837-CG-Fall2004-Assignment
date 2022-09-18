# assignment7
SuperSampling:  
    <-1-> Random_Sample:  
    task.json	
    "args": 
        +"-std=c++17",//to support C++17  

    #include < iostream >  
    #include < random >  
    #include < iomanip >  
    constexpr int FLOAT_MIN = 0;  
    constexpr int FLOAT_MAX = 1;  
    std::random_device rd;  
    std::default_random_engine eng(rd());  
    std::uniform_real_distribution<float> distr(FLOAT_MIN, FLOAT_MAX);  

    for (int n = 0; n < 5; ++n) {  
            cout << setprecision(6)  
                << distr(eng) << "\n";  
        }  
    <-2-> Uniform_Sample  
    <-3-> jittered_samples,i.e. Random+Uniform samples  

Debug:make sure that Command-Line arguments and their sequence are correct  


Filter and Antialiasing  
    -box/tent/gaussian filter  
    Reference: Fundamentals of computer graphics-(2009)  Ch9 SignalProcessing  