    #include <iostream>
    #include <string>

    typedef unsigned char uchar;

    class Image
    {
    public:

        Image(const std::string& filename);
        ~Image();

        int width() const { return m_width; }
        int height() const { return m_height; }

        uchar* data() { return m_data; }
        const uchar* data() const { return m_data; }
        uchar* data(int y) { return m_data + y * m_width; }
        const uchar* data(int y) const { return m_data + y * m_width; }

        void adjust_brightness(int delta);
        void adjust_contrast(float factor);

        float* create_gaussian_kernel(float sigma, int n);

        void smooth_x(float sigma, int n);
        void smooth_y(float sigma, int n);
        void smooth(float sigma_x, int n_x, float sigma_y, int n_y);
        
        short* deriv_x(bool pre_smooth);
        short* deriv_y(bool pre_smooth);
        void edge_detection(short threshold, bool pre_smooth);

        void rotate(float theta, int rotation_x = 0, int rotation_y = 0);

        void load_pgm(const std::string& filename);
        void write_pgm(const std::string& filename);    

    private:
        int m_width, m_height;
        uchar* m_data;

        uchar* copy_row_to_buffer(const uchar* row, int border_size);
        uchar* copy_column_to_buffer(const uchar* column, int border_size);
        void smooth_buffer(uchar* buffer, float* gaussian_kernel, int buffer_size ,int border_size);
        short* smooth_deriv_buffer(uchar* buffer, short* kernel, int buffer_size, int border_size);
    };