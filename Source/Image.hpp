#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>

/// <summary>
/// Defines an image.
/// </summary>
class Image
{
    friend class Texture2D;

    std::vector<unsigned char> _pixels;
    unsigned int _width;
    unsigned int _height;

    /// <summary>
    /// Disposes of any current pixel information.
    /// </summary>
    void Dispose();

public:
    /// <summary>
    /// Creates a new image.
    /// </summary>
    Image();

    /// <summary>
    /// Destroys this image.
    /// </summary>
    ~Image();

    /// <summary>
    /// Gets this image's height.
    /// </summary>
    unsigned int GetHeight() const;

    /// <summary>
    /// Gets this image's width.
    /// </summary>
    unsigned int GetWidth() const;

    /// <summary>
    /// Gets the pixel at the given coordinates.
    /// </summary>
    /// <param name="x">The X coordinate to retrieve the pixel at.</param>
    /// <param name="y">The Y coordinate to retrieve the pixel at.</param>
    DirectX::XMFLOAT4 GetPixel( unsigned int x, unsigned int y ) const;

    /// <summary>
    /// Attempts to load image data from the given file.
    /// </summary>
    /// <param name="fname">The file name.</param>
    bool LoadFromFile( const std::string& fname );
};
