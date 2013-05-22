//
//  PlayerViewController.h
//  carbonarrow
//
//  Created by Archer_LJ on 13-5-22.
//  Copyright (c) 2013年 Archer_LJ. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "DOUAudioStreamer.h"

@interface PlayerViewController : UIViewController

- (void)StartPlay:(NSString*)trackname;

- (void)Play;
- (void)Pause;
- (void)Stop;

- (double)GetVolume;
- (void)SetVolume:(double)volume;

@end
