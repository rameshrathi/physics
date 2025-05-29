
class FileSystem {
    public:
        FileSystem(const char * path) : fs_path_(path) {}
        ~FileSystem() {}
        
        const char * allFiles();
    
    private:
       const char * fs_path_;
};