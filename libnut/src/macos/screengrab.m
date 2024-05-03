#include "../screengrab.h"
#include "../endian.h"
#include <stdlib.h> /* malloc() */

#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>
#import <Cocoa/Cocoa.h>

static double getPixelDensity() {
    @autoreleasepool
    {
        NSScreen * mainScreen = [NSScreen
        mainScreen];
        if (mainScreen) {
            return mainScreen.backingScaleFactor;
        } else {
            return 1.0;
        }
    }
}

bool captureScreenAreaToJPEG(MMRect rect, const char *outputPath, float compressionQuality) {
    CGDirectDisplayID displayID = CGMainDisplayID();
    CGImageRef image = CGDisplayCreateImageForRect(displayID, CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height));

    if (!image) return false;

    CFStringRef outputPathString = CFStringCreateWithCString(NULL, outputPath, kCFStringEncodingUTF8);
    CFURLRef url = CFURLCreateWithFileSystemPath(NULL, outputPathString, kCFURLPOSIXPathStyle, false);
    CGImageDestinationRef destination = CGImageDestinationCreateWithURL(url, kUTTypeJPEG, 1, NULL);
    if (!destination) {
        CGImageRelease(image);
        CFRelease(url);
        return false;
    }

    // Specify the compression quality for the JPEG file (0.0 to 1.0)
    CFDictionaryRef options = (__bridge CFDictionaryRef)@{(id)kCGImageDestinationLossyCompressionQuality: @(compressionQuality)};
    CGImageDestinationAddImage(destination, image, options);

    bool success = CGImageDestinationFinalize(destination);

    CGImageRelease(image);
    CFRelease(destination);
    CFRelease(url);

    return success;
}

bool captureAndResizeScreenAreaToJPEG(MMRect rect, const char *outputPath, float compressionQuality) {

    double pixelDensity = getPixelDensity();
    if (pixelDensity == 1.0) {
        return captureScreenAreaToJPEG(rect, outputPath, compressionQuality);
    }

    CGDirectDisplayID displayID = CGMainDisplayID();
    CGImageRef originalImage = CGDisplayCreateImageForRect(displayID, CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height));

    if (!originalImage) return false;

    // Calculate new dimensions based on pixel density
    size_t newWidth = (size_t)(CGImageGetWidth(originalImage) / pixelDensity);
    size_t newHeight = (size_t)(CGImageGetHeight(originalImage) / pixelDensity);

    // Create a context to draw the resized image
    CGContextRef context = CGBitmapContextCreate(NULL, newWidth, newHeight,
                                                 CGImageGetBitsPerComponent(originalImage),
                                                 0,
                                                 CGImageGetColorSpace(originalImage),
                                                 CGImageGetBitmapInfo(originalImage));
    if (!context) {
        CGImageRelease(originalImage);
        return false;
    }

    // Draw the original image into the context, resizing it
    CGContextSetInterpolationQuality(context, kCGInterpolationHigh);
    CGContextDrawImage(context, CGRectMake(0, 0, newWidth, newHeight), originalImage);
    CGImageRef resizedImage = CGBitmapContextCreateImage(context);

    // Prepare to write the JPEG file
    CFStringRef outputPathString = CFStringCreateWithCString(NULL, outputPath, kCFStringEncodingUTF8);
    CFURLRef url = CFURLCreateWithFileSystemPath(NULL, outputPathString, kCFURLPOSIXPathStyle, false);
    CGImageDestinationRef destination = CGImageDestinationCreateWithURL(url, kUTTypeJPEG, 1, NULL);
    if (!destination) {
        CGImageRelease(originalImage);
        CGImageRelease(resizedImage);
        CFRelease(url);
        CGContextRelease(context);
        return false;
    }

    // Specify JPEG compression quality
    CFDictionaryRef options = (__bridge CFDictionaryRef)@{(id)kCGImageDestinationLossyCompressionQuality: @(compressionQuality)};
    CGImageDestinationAddImage(destination, resizedImage, options);

    // Finalize the JPEG file writing
    bool success = CGImageDestinationFinalize(destination);

    // Cleanup
    CGImageRelease(originalImage);
    CGImageRelease(resizedImage);
    CFRelease(destination);
    CFRelease(url);
    CGContextRelease(context);

    return success;
}

MMBitmapRef copyMMBitmapFromDisplayInRect(MMRect rect) {

    CGDirectDisplayID displayID = CGMainDisplayID();

    CGImageRef image = CGDisplayCreateImageForRect(displayID,
                                                   CGRectMake(
                                                           rect.origin.x,
                                                           rect.origin.y,
                                                           rect.size.width,
                                                           rect.size.height
                                                   )
    );

    if (!image) { return NULL; }

    CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(image));

    if (!imageData) { return NULL; }

    long bufferSize = CFDataGetLength(imageData);
    size_t bytesPerPixel = (size_t) (CGImageGetBitsPerPixel(image) / 8);
    double pixelDensity = getPixelDensity();
    long expectedBufferSize = rect.size.width * pixelDensity * rect.size.height * pixelDensity * bytesPerPixel;

    if (expectedBufferSize < bufferSize) {
        size_t reportedByteWidth = CGImageGetBytesPerRow(image);
        size_t expectedByteWidth = expectedBufferSize / (rect.size.height * pixelDensity);

        uint8_t *buffer = malloc(expectedBufferSize);

        const uint8_t *dataPointer = CFDataGetBytePtr(imageData);
        size_t parts = bufferSize / reportedByteWidth;

        for (size_t idx = 0; idx < parts - 1; ++idx) {
            memcpy(buffer + (idx * expectedByteWidth),
                   dataPointer + (idx * reportedByteWidth),
                   expectedByteWidth
            );
        }

        MMBitmapRef bitmap = createMMBitmap(buffer,
                                            rect.size.width * pixelDensity,
                                            rect.size.height * pixelDensity,
                                            expectedByteWidth,
                                            CGImageGetBitsPerPixel(image),
                                            CGImageGetBitsPerPixel(image) / 8);

        CFRelease(imageData);
        CGImageRelease(image);

        return bitmap;
    } else {
        uint8_t *buffer = malloc(bufferSize);
        CFDataGetBytes(imageData, CFRangeMake(0, bufferSize), buffer);
        MMBitmapRef bitmap = createMMBitmap(buffer,
                                            CGImageGetWidth(image),
                                            CGImageGetHeight(image),
                                            CGImageGetBytesPerRow(image),
                                            CGImageGetBitsPerPixel(image),
                                            CGImageGetBitsPerPixel(image) / 8);

        CFRelease(imageData);

        CGImageRelease(image);

        return bitmap;
    }
}
