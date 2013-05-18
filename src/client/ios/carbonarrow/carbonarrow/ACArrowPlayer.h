//
//  ACArrowPlayer.h
//  carbonarrow
//
//  Created by Archer_LJ on 13-5-18.
//  Copyright (c) 2013年 Archer_LJ. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "DOUAudioStreamer.h"

@interface ACArrowPlayer : UIViewController
{
    DOUAudioStreamer* _streamer;
}

- (void)InitPlayer:(NSString*)filename;

- (void)Play;
- (void)Pause;

- (NSTimeInterval)GetCurrentPos;
- (NSTimeInterval)GetDuration;

- (void)Mute;
- (double)GetVolume;
- (void)SetVolume:(double)value;

@end
