// Lightweight image loader implementation for macOS using Image I/O and
// CoreGraphics. We implement a small subset of the stb_image API used by
// the project so we can avoid adding the full stb_image single-header
// implementation into the repo. Provide C linkage to match the header
// declarations (stb_image.h uses extern "C").

#include <CoreFoundation/CoreFoundation.h>
#include <ImageIO/ImageIO.h>
#include <CoreGraphics/CoreGraphics.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned char *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels)
{
	if(!filename) return NULL;

	// Create a CFURL from the filesystem path
	CFStringRef cfPath = CFStringCreateWithCString(NULL, filename, kCFStringEncodingUTF8);
	if(!cfPath) return NULL;
	CFURLRef url = CFURLCreateWithFileSystemPath(NULL, cfPath, kCFURLPOSIXPathStyle, false);
	CFRelease(cfPath);
	if(!url) return NULL;

	CGImageSourceRef src = CGImageSourceCreateWithURL(url, NULL);
	CFRelease(url);
	if(!src) return NULL;

	CGImageRef image = CGImageSourceCreateImageAtIndex(src, 0, NULL);
	CFRelease(src);
	if(!image) return NULL;

	size_t width = CGImageGetWidth(image);
	size_t height = CGImageGetHeight(image);
	int components = 4; // We'll return RGBA

	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	if(!colorSpace) { CGImageRelease(image); return NULL; }

	// kCGImageAlphaPremultipliedLast matches RGBA ordering for CoreGraphics
	CGBitmapInfo bitmapInfo = kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big;
	size_t bytesPerRow = width * components;
	unsigned char *data = (unsigned char*)malloc(height * bytesPerRow);
	if(!data) { CGColorSpaceRelease(colorSpace); CGImageRelease(image); return NULL; }

	CGContextRef ctx = CGBitmapContextCreate(data, width, height, 8, bytesPerRow, colorSpace, bitmapInfo);
	if(!ctx) { free(data); CGColorSpaceRelease(colorSpace); CGImageRelease(image); return NULL; }

	// Draw the image into our buffer (this will convert formats as needed)
	CGRect rect = CGRectMake(0, 0, (double)width, (double)height);
	CGContextDrawImage(ctx, rect, image);

	CGContextRelease(ctx);
	CGImageRelease(image);
	CGColorSpaceRelease(colorSpace);

	if(x) *x = (int)width;
	if(y) *y = (int)height;
	if(channels_in_file) *channels_in_file = components;

	// We always return RGBA (4 channels). Ignore desired_channels for simplicity.
	return data;
}

void stbi_image_free(void *retval_from_stbi_load)
{
	if(retval_from_stbi_load) free(retval_from_stbi_load);
}

// Minimal implementations of flip control functions used by the sample.
// These are no-ops for this simple implementation.
static int stbi__flip_vertically_on_load = 0;

void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip)
{
	stbi__flip_vertically_on_load = flag_true_if_should_flip ? 1 : 0;
}

void stbi_set_flip_vertically_on_load_thread(int flag_true_if_should_flip)
{
	stbi_set_flip_vertically_on_load(flag_true_if_should_flip);
}

#ifdef __cplusplus
}
#endif
