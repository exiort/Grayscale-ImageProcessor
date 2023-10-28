    #include "Image.hpp"
    #include <stdexcept>
    #include <algorithm>
    #include <cmath>
    #include <cstring>
    #include <fstream>

    using namespace std;

    Image::Image(const std::string& filename)
    {
        m_data = nullptr;
        load_pgm(filename);
    }

    Image::~Image()
    {
        delete[] m_data;
    }

    void Image::adjust_brightness(int delta)//checked!
    {   
        if (delta < -255 || delta > 255) {
            throw invalid_argument("Value must be between -255, 255! Provided:" + to_string(delta));
        }

        uchar* img = data();
        for (int i = 0; i < m_width * m_height; i++) {
            int value =  img[i] + delta;
            img[i] =  static_cast<uchar>(max(0, min(255, value)));
        }
    }

    void Image::adjust_contrast(float factor) //checked!
    {
        if (factor < 0) throw invalid_argument("Factor value must be non-negative! Provided:" + to_string(factor));

        uchar* img = data();
        for (int i = 0; i < width() * height(); i++) {
            int value = img[i] * factor;
            img[i] = static_cast<uchar>(max(0, min(255, value))); 
        }
    }

    float* Image::create_gaussian_kernel(float sigma, int n)
    {   
        if(sigma < 0) throw invalid_argument("Sigma value must be non-negative! Provided:" + to_string(sigma));
        if(n > m_width / 2) throw invalid_argument("n value cannot be exceed half of the image which is:" + to_string(m_width / 2) + " Provided:" + to_string(n));

        int size = 2 * n + 1;
        float* gaussian_kernel = new float[size];

        double sum = 0;

        for (int i = 0; i < size; i++) {
            int x = i - n;
            gaussian_kernel[i] = exp(-(pow(x,2) / (2 * pow(sigma, 2))));
            sum += gaussian_kernel[i];
        }

        for (int i = 0; i < size; i++) {
            gaussian_kernel[i] /= sum;
        }   

        return gaussian_kernel;
    }

    uchar* Image::copy_row_to_buffer(const uchar* row, int border_size)
    {
        uchar* buffer = new uchar[m_width + 2 * border_size];
        memcpy(buffer + border_size, row, sizeof(*row) * width());
        
        for (int x = 0; x < border_size; x++) {
            buffer[x] = buffer[2 * border_size - x];
            buffer[m_width + 2 * border_size - 1 - x] = buffer[m_width - 1 + x];    
        }   
        
        return buffer;
    }

    uchar* Image::copy_column_to_buffer(const uchar* column, int border_size) 
    {   
        uchar* buffer = new uchar[m_height + 2 * border_size];

        for (int y = 0; y < m_height; y++) {
            buffer[y + border_size] = column[y * width()];
        }

        for (int y = 0; y < border_size; y++) {
            buffer[y] = buffer[2 * border_size - y];
            buffer[m_height + 2 * border_size - 1 - y] = buffer[m_height - 1 + y];    
        }   

        return buffer;
    }

    void Image::smooth_buffer(uchar* buffer, float* gaussian_kernel, int buffer_size, int border_size)
    {
        for (int i = border_size; i <= buffer_size + border_size - 1; i++) {
            double value = 0;
            int gaussian_index = 0;
            for (int j = i - border_size; j <= i + border_size; j++) {
                value += buffer[j] * gaussian_kernel[gaussian_index++];
            }
            buffer[i - border_size] = static_cast<uchar>(value);
        }
    }

    void Image::smooth_x(float sigma, int n)
    {   
        float* gaussian_kernel = create_gaussian_kernel(sigma, n);
        uchar* row;
        uchar* buffer; 
        for (int y = 0; y < height(); y++) {
            row = data(y);
            buffer = copy_row_to_buffer(row, n);
            smooth_buffer(buffer, gaussian_kernel, width(), n);
            memcpy(row, buffer, width() * sizeof(*row));
            delete[] buffer;
        }
        
        
        delete[] gaussian_kernel;
    }

    void Image::smooth_y(float sigma, int n)
    {
        float* gaussian_kernel = create_gaussian_kernel(sigma, n);
        uchar* column;
        uchar* buffer;
        for (int x = 0; x < width(); x++) {
            column = data() + x;
            buffer = copy_column_to_buffer(column, n);
            smooth_buffer(buffer, gaussian_kernel, height(), n);

            for (int y = 0; y < height(); y++) {
                column[y * width()] = buffer[y];
            }

            delete[] buffer;
        }   
        delete[] gaussian_kernel;
    }

    void Image::smooth(float sigma_x, int n_x, float sigma_y, int n_y)
    {
        smooth_x(sigma_x, n_x);
        smooth_y(sigma_y, n_y);
    }


    short* Image::deriv_x(bool pre_smooth)
    {
        if (pre_smooth) smooth_y(0.9,1);

        short* derived_data = new short[m_width * m_height];
        short* kernel = new short[3] {-1, 0, 1};
        
        uchar* buffer;
        const uchar* row;

        for(int y = 0; y < m_height; y++) {
            row = data(y);
            buffer = copy_row_to_buffer(row, 1);

            for (int x = 1; x < m_width + 1; x++) {
                short value = 0;
                int kernel_index = 0; 

                for (int i = x - 1; i <= x + 1; i++) {
                    value += buffer[i] * kernel[kernel_index++];
                }
                derived_data[y * m_width + x - 1] = value;
            }
            delete[] buffer;
        }

        delete[] kernel;

        return derived_data;
    }

    short* Image::deriv_y(bool pre_smooth)
    {
        if (pre_smooth) smooth_x(0.9, 1);

        short* derived_data = new short[m_width * m_height];
        short* kernel = new short[3] {-1, 0, 1};

        const uchar* column;
        uchar* buffer;
        for (int x = 0; x < m_width; x++) {
            column = data() + x;
            buffer = copy_column_to_buffer(column, 1);

            for (int y = 1; y < m_height + 1; y++) {
                short value = 0;
                int kernel_index = 0; 

                for (int i = y - 1; i <= y + 1; i++) {
                    value += buffer[i] * kernel[kernel_index++];
                }
                derived_data[(y - 1) * m_width + x] = value;
            }
            delete[] buffer;
        }

        delete[] kernel;
        

        return derived_data;
    }

    void Image::edge_detection(short threshold, bool pre_smooth)
    {   
        if (threshold < 10 || threshold > 250) throw invalid_argument("Threshhold value must be between 10 and 250! Provided:" + to_string(threshold));

        short* Ix = deriv_x(pre_smooth);
        short* Iy = deriv_y(pre_smooth);
        
        uchar value;
        for(int i = 0; i < m_width * m_height; i++) {
            value = sqrt((pow(Ix[i], 2) + pow(Iy[i], 2)));
            if (value >= threshold) m_data[i] = 0;
            else m_data[i] = 255;
        }

        delete[] Ix;
        delete[] Iy;
    }

    void Image::rotate(float theta, int rotation_x, int rotation_y)
    {
        if(rotation_x >= m_width || rotation_x < 0 || rotation_y >= m_height || rotation_y < 0) {
            throw invalid_argument("Rotation pixel must be inside of Image! Image size:" + to_string(m_width) + "-" + to_string(m_height) + "! Provided: " + to_string(rotation_x) + "-" + to_string(rotation_y));
        }

        int x_out, y_out, x_in, y_in;

        const float sintheta = sin(theta);
        const float costheta = cos(theta);
        
        uchar* out = new uchar[m_width * m_height];
        uchar* row_out;

        for (y_out = 0; y_out < m_height; y_out++) {
            row_out = out + y_out * m_width;
            for(x_out = 0; x_out < m_width; x_out++) {
                x_in = static_cast<int>(round(costheta * (x_out - rotation_x) + sintheta * (y_out - rotation_y))) + rotation_x;
                y_in = static_cast<int>(round(-sintheta * (x_out - rotation_x) + costheta * (y_out - rotation_y))) + rotation_y;

                if(x_in < 0 || x_in >= m_width || y_in < 0 || y_in >= m_height) out[y_out * m_width + x_out] = 0;

                else out[y_out * m_width + x_out] = m_data[y_in * m_width + x_in];
            }


        }
        delete[] m_data;
        m_data = out;
    }

    void Image::load_pgm(const std::string& filename)
    {
        ifstream fin(filename, std::ios::binary);
        
        if(!fin) {
                throw runtime_error("File not founded!");
        }
        std::string magic_head;
        fin >> magic_head;
        
        if (magic_head != "P5") {
           throw runtime_error("File format is not supported. It must be a pgm image");
        }

        fin >> m_width;
        fin >> m_height;
        int max_value;
        fin >> max_value;
        
        if (max_value != 255) {
            cerr << "Error!" << endl;
            exit(1);
        }   
        if (m_data != nullptr) delete[] m_data;
        m_data = new  uchar[m_width * m_height];
        
        for (int i = 0; i < m_width * m_height; i++) {
            m_data[i] = fin.get();
        }     
        fin.close();
    }

    void Image::write_pgm(const std::string& filename)
    {
        string magic_head = "P5";
        string extended_name = filename + ".pgm";
        ofstream fout;

        fout.open(extended_name.c_str(), ios::out | ios::binary);
        fout << magic_head << endl;
        fout << m_width << " " << m_height << endl;
        fout << "255" << endl;

        for (int i = 0; i < m_width * m_height; i++) {
            fout.put(m_data[i]);
        }

        fout.close();
    }

