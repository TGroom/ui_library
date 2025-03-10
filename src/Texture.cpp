/*
* Modified from: https://learnopengl.com/In-Practice/2D-Game/Rendering-Sprites
*/

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "ui_library/Texture.h"

Texture2D::Texture2D(GLenum internalFormat, GLenum imageFormat, const std::filesystem::path& file, glm::vec2 boundingBox)
    : width(0), height(0), wrapS(GL_REPEAT), wrapT(GL_REPEAT),
      filterMin(GL_LINEAR), filterMax(GL_LINEAR), 
      internalFormat(internalFormat), imageFormat(imageFormat), ID(0) {

    std::vector<GLfloat> vertices = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 
    
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    VBO1.Data(vertices);
    VAO1.Bind();
    VAO1.LinkAttrib(VBO1, 0, 4, GL_FLOAT, 4 * sizeof(float), (void*)0);
    VAO1.Unbind();
    // load shaders
    spriteShader.Set((std::string(UI_LIBRARY_RESOURCES_DIR) + "/shaders/Sprite.vert").c_str(), (std::string(UI_LIBRARY_RESOURCES_DIR) + "/shaders/Sprite.frag").c_str());//, nullptr, "sprite");
    // configure shaders
    spriteShader.Bind().SetInteger("image", 0);

    loadTextureFromFileAsync(file, boundingBox);

    spriteShader.Unbind();
}



void Texture2D::Create(GLuint texWidth, GLuint texHeight, const unsigned char* pixelData) {
    if (ID == 0) {
        glGenTextures(1, &ID); // Generate texture ID if it doesn't exist
    }

    width = texWidth;
    height = texHeight;

    Bind();

    // Allocate texture storage and copy data
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, pixelData);

    // Configure texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMax);

    Unbind(); // Unbind texture
}


void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}


void Texture2D::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}



// Helper function to load image
unsigned char* loadImage(const std::string& filePath, int& originalWidth, int& originalHeight, int& channels) {
    unsigned char* imageData = stbi_load(filePath.c_str(), &originalWidth, &originalHeight, &channels, 4); // Force 4 channels (RGBA)
    if (!imageData) {
        std::cerr << "Could not open or find the image: " << filePath << std::endl;
        return nullptr;
    }
    return imageData;
}

// Helper function to resize image
unsigned char* resizeImage(unsigned char* imageData, int originalWidth, int originalHeight, glm::vec2& boundingBox) {
    // Calculate the aspect ratio of the original image and the bounding box
    float imageAspectRatio = static_cast<float>(originalWidth) / static_cast<float>(originalHeight);
    float boxAspectRatio = boundingBox.x / boundingBox.y;

    // Determine the new width and height based on the aspect ratio comparison
    int newWidth, newHeight;

    if (imageAspectRatio > boxAspectRatio) {
        // Image is wider than the box, limit by width
        newWidth = static_cast<int>(boundingBox.x);
        newHeight = static_cast<int>(boundingBox.x / imageAspectRatio);
        if (newHeight > boundingBox.y) {
            // If the new height exceeds the bounding box, adjust based on height
            newHeight = static_cast<int>(boundingBox.y);
            newWidth = static_cast<int>(boundingBox.y * imageAspectRatio);
        }
    } else {
        // Image is taller than the box or fits proportionally, limit by height
        newHeight = static_cast<int>(boundingBox.y);
        newWidth = static_cast<int>(boundingBox.y * imageAspectRatio);
        if (newWidth > boundingBox.x) {
            // If the new width exceeds the bounding box, adjust based on width
            newWidth = static_cast<int>(boundingBox.x);
            newHeight = static_cast<int>(boundingBox.x / imageAspectRatio);
        }
    }

    // Update the bounding box input to reflect the new dimensions
    boundingBox = glm::vec2(newWidth, newHeight);

    // Allocate memory for the resized thumbnail
    unsigned char* thumbnailData = new unsigned char[newWidth * newHeight * 4]; // Assuming 4 channels (RGBA)

    // Resize the image using stb_image_resize2
    stbir_resize(
        imageData,                     // Input image
        originalWidth, originalHeight, // Input dimensions
        originalWidth * 4,             // Input stride (width * channels)
        thumbnailData,                 // Output image
        newWidth, newHeight,           // Output dimensions
        newWidth * 4,                  // Output stride (width * channels)
        STBIR_RGBA,                    // Pixel layout
        STBIR_TYPE_UINT8,              // Data type
        STBIR_EDGE_CLAMP,              // Edge handling
        STBIR_FILTER_DEFAULT           // Default filter for resizing
    );
    
    return thumbnailData;
}


// Refactored loadTextureFromFile (Async with callback)
// Load texture asynchronously and pass the result to the main thread queue
void Texture2D::loadTextureFromFileAsync(const std::filesystem::path& file, glm::vec2 boundingBox) {
    std::thread([this, file, boundingBox]() {  // Capture self
        // Load and resize the image as before...
        std::string filePath = file.string();
        int originalWidth, originalHeight, channels;
        unsigned char* imageData = loadImage(filePath, originalWidth, originalHeight, channels);
        if (!imageData) {
            return;
        }

        glm::vec2 constrainedBox = boundingBox;
        unsigned char* thumbnailData = resizeImage(imageData, originalWidth, originalHeight, constrainedBox);
        stbi_image_free(imageData);

        if (!thumbnailData) {
            return;
        }

        // Queue the texture data for processing on the main thread
        {
            if (!this) {
                return;
            }
            // Debug: log mutex address or status if possible
            std::lock_guard<std::mutex> lock(queueMutex);
            textureQueue.push(std::make_tuple(thumbnailData, constrainedBox));  // Help me debug an ERROR HERE
        }

        queueCondVar.notify_one();  // Notify the main thread to process the queue
    }).join();  // TODO: Figure out a way to make this be .detach(); but that dosn't crash the program
}


void Texture2D::processTextureQueue() {
    std::unique_lock<std::mutex> lock(queueMutex);
    while (!textureQueue.empty()) {
        auto [thumbnailData, constrainedBox] = textureQueue.front();
        textureQueue.pop();
        lock.unlock(); // Unlock the mutex so other threads can add data

        if (!thumbnailData) {
            std::cerr << "Invalid thumbnail data" << std::endl;
            return;
        }

        // Set texture format
        internalFormat = GL_RGBA; // RGBA format
        imageFormat = GL_RGBA;

        // Generate the texture on the main thread
        Create(constrainedBox.x, constrainedBox.y, thumbnailData);
        mFitSize = constrainedBox;

        // Cleanup
        delete[] thumbnailData;

        lock.lock(); // Lock the mutex again to continue processing the queue
    }
}


void Texture2D::DrawSprite(glm::vec2 position, glm::vec2 desiredSize, float z, float rotate, glm::vec3 color)
{
    processTextureQueue();

    mDesiredSize = desiredSize;

    // Prepare transformations
    spriteShader.Bind();
    int wWidth = 0;
    int wHeight = 0;
    glfwGetFramebufferSize(G_WINDOW, &wWidth, &wHeight);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(wWidth), static_cast<float>(wHeight), 0.0f, -1.0f, 1.0f);
    spriteShader.SetMatrix4("projection", projection);

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(position + ((mDesiredSize - mFitSize) / 2.0f), z));

    //model = glm::translate(model, glm::vec3(0.5f * mFitSize.x, 0.5f * mFitSize.y, 0.0f)); 
    //model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); 
    //model = glm::translate(model, glm::vec3(-0.5f * mFitSize.x, -0.5f * mFitSize.y, 0.0f));

    model = glm::scale(model, glm::vec3(mFitSize, 1.0f)); 
  
    spriteShader.SetMatrix4("model", model);
    spriteShader.SetVector3f("spriteColor", color);
  
    glActiveTexture(GL_TEXTURE0);
    this->Bind();

    VAO1.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    VAO1.Unbind();

    this->Unbind();
    spriteShader.Unbind();
}  


//void Texture2D::setIcon(std::filesystem::path iconPath, int iconX, int iconY, int iconW, int iconH) {
//    mIconX = iconX;
//    mIconY = iconY;
//    mIconW = iconW;
//    mIconH = iconH;
//    
//    if (mIconPath != iconPath) {
//        mIconPath = iconPath;
//        if (!mIconPath.empty()) {
//            loadTextureFromFileAsync(iconTexture, mIconPath, glm::vec2(mIconW, mIconH));
//        }
//    }
//}

//uint SpriteRenderer::loadTextureFromFile(std::shared_ptr<Texture2D>& texture, const std::filesystem::path& file, uint width, uint height) {
//    // Convert std::filesystem::path to a C-style string (const char*)
//    std::string filePath = file.string();
//
//    // Load the image using stb_image
//    int originalWidth, originalHeight, channels;
//    unsigned char* imageData = stbi_load(filePath.c_str(), &originalWidth, &originalHeight, &channels, 4); // Force 4 channels (RGBA)
//    if (!imageData) {
//        printf("Could not open or find the image: %s\n", filePath.c_str());
//        return height;
//    }
//
//    // Calculate the thumbnail dimensions while maintaining the aspect ratio
//    float aspectRatio = static_cast<float>(originalHeight) / static_cast<float>(originalWidth);
//    uint thumbnailHeight = static_cast<uint>(width * aspectRatio);
//
//    // Allocate memory for the resized thumbnail
//    unsigned char* thumbnailData = new unsigned char[width * thumbnailHeight * 4]; // Assuming 4 channels (RGBA)
//
//    // Resize the image using stb_image_resize2
//    stbir_resize(
//        imageData,                     // Input image
//        originalWidth, originalHeight, // Input dimensions
//        originalWidth * 4,             // Input stride (width * channels)
//
//        thumbnailData,                 // Output image
//        width, thumbnailHeight,        // Output dimensions
//        width * 4,                     // Output stride (width * channels)
//
//        STBIR_RGBA,       // Pixel layout
//        STBIR_TYPE_UINT8,              // Data type
//        STBIR_EDGE_CLAMP,              // Edge handling
//        STBIR_FILTER_DEFAULT           // Default filter for resizing
//    );
//    
//    // Assume resizing was successful; validate output buffer (optional)
//    if (!thumbnailData) {
//        printf("Thumbnail resizing failed\n");
//        stbi_image_free(imageData);
//        delete[] thumbnailData;
//        return height;
//    }
//
//    // Set texture format
//    texture->Internal_Format = GL_RGBA; // RGBA format
//    texture->Image_Format = GL_RGBA;
//
//    // Generate the texture
//    texture->Generate(width, thumbnailHeight, thumbnailData);
//
//    // Cleanup
//    stbi_image_free(imageData);
//    delete[] thumbnailData;
//
//    return thumbnailHeight;
//}


//uint SpriteRenderer::loadTextureFromFile(std::shared_ptr<Texture2D>& texture, std::wstring file, uint width, uint height) {
//    // Initialize GDI+
//    GdiplusStartupInput gdiplusStartupInput;
//    ULONG_PTR gdiplusToken;
//    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
//
//    // Load the image from the specified file
//    Image* image = new Image(file.c_str());
//    if (image->GetLastStatus() != Ok) {
//        wprintf(L"Could not open or find the image: %ls\n", file.c_str());
//        GdiplusShutdown(gdiplusToken);
//        return height;
//    }
//
//    // Ensure that the thumbnail has the right aspect ratio
//    uint originalWidth = image->GetWidth();
//    uint originalHeight = image->GetHeight();
//    float aspectRatio = static_cast<float>(originalHeight) / static_cast<float>(originalWidth);
//    uint thumbnailHeight = static_cast<uint>(width * aspectRatio);
//
//    Image* thumbnail = image->GetThumbnailImage(width, thumbnailHeight, nullptr, nullptr);
//    if (!thumbnail || thumbnail->GetLastStatus() != Ok) {
//        printf("Failed to create thumbnail\n");
//        delete image;
//        GdiplusShutdown(gdiplusToken);
//        return height;
//    }
//
//    // Get thumbnail properties
//    BitmapData bitmapData;
//    Gdiplus::Rect rect(0, 0, width, thumbnailHeight);
//
//    // Lock the bits of the image for direct access
//    Bitmap* bitmap = static_cast<Bitmap*>(thumbnail);
//    bitmap->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);
//
//    // Get image data
//    unsigned char* data = new unsigned char[width * thumbnailHeight * 4]; // Assuming 4 channels (RGBA)
//    std::memcpy(data, bitmapData.Scan0, width * thumbnailHeight * 4);
//
//    // Unlock the bits
//    bitmap->UnlockBits(&bitmapData);
//
//    // Set texture format
//    texture->Internal_Format = GL_BGRA;
//    texture->Image_Format = GL_BGRA;
//
//    // Generate the texture
//    texture->Generate(width, thumbnailHeight, data);
//
//    // Cleanup
//    delete[] data;
//    delete thumbnail;
//    delete image;
//    GdiplusShutdown(gdiplusToken);
//    return thumbnailHeight;
//}




// METHOD FOR LOADING THUMBNAIL USING stbi_load AND OPENCV VVVVV
//uint SpriteRenderer::loadTextureFromFile(std::shared_ptr<Texture2D>& texture, const char *file)
//{
//    printf("\n-------\n");
//    printf(file);
//    printf("\n-------\n");
//
//    // Load image with stb_image
//    int width, height, nrChannels;
//    unsigned char* data = NULL;//stbi_load(file, &width, &height, &nrChannels, 0);
//
//    // If stb_image fails, use OpenCV to load the image
//    if (!data) {
//        // Load image using OpenCV
//        cv::Mat image = cv::imread(file, cv::IMREAD_UNCHANGED); // Load image with alpha if available
//
//        // Check if the image was loaded successfully
//        if (image.empty()) {
//            std::cerr << "Could not open or find the image: " << file << std::endl;
//            return;
//        }
//
//        cv::resize(image, image, cv::Size(90, 90), 0, 0, cv::INTER_LINEAR);
//
//        nrChannels = image.channels();
//        // OpenCV loads in BGR, so convert to RGB if it's not a grayscale or alpha image
//        if (nrChannels == 3) {
//            cv::cvtColor(image, image, cv::COLOR_BGR2RGB); // Convert BGR to RGB
//        } else if (nrChannels == 4) {
//            cv::cvtColor(image, image, cv::COLOR_BGRA2RGBA); // Convert BGRA to RGBA
//        }
//        // Convert cv::Mat to unsigned char*
//        width = image.cols;
//        height = image.rows;
//        data = new unsigned char[width * height * nrChannels]; // Allocate memory for the image data
//        std::memcpy(data, image.data, width * height * nrChannels); // Copy data from cv::Mat to raw pointer
//    }
//
//    // Determine the internal format based on the number of channels
//    if (nrChannels == 4) { // RGBA
//        texture->Internal_Format = GL_RGBA;
//        texture->Image_Format = GL_RGBA;
//    } else { // RGB
//        texture->Internal_Format = GL_RGB;
//        texture->Image_Format = GL_RGB;
//    }
//
//    // Now generate the texture
//    texture->Generate(width, height, data);
//
//    // Free image data if it was loaded with stb_image
//    if (data != nullptr) {
//        stbi_image_free(data);
//    } else {
//        delete[] data; // Free memory allocated for OpenCV image data
//    }
//    return height;
//}
// 
//    // load image
//    int width, height, nrChannels;
//    //stbi_set_flip_vertically_on_load(false);
//    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
//    // now generate texture
//    texture->Generate(width, height, data);
//    // and finally free image data
//    stbi_image_free(data);
//}