//
//  ACArrowPlayer.m
//  carbonarrow
//
//  Created by Archer_LJ on 13-5-18.
//  Copyright (c) 2013年 Archer_LJ. All rights reserved.
//

#import "ACArrowPlayer.h"

@interface Track : NSObject <DOUAudioFile>
@property (nonatomic, strong) NSURL* url;
@end

@implementation Track

- (NSURL*)audioFileURL
{
    return [self url];
}

@end


@interface ACArrowPlayer ()

@end

@implementation ACArrowPlayer

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
}

- (void)viewDidUnload
{
    // do things
    
    [super viewDidUnload];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)InitPlayer:(NSString*)filename
{
    if(_streamer != nil)
    {
        [_streamer pause];
        _streamer = nil;
    }
    
    NSString* keyword = @"http:";
    
    if([filename rangeOfString:keyword].location != NSNotFound)
    {
        Track* track = [[[Track alloc]init]autorelease];
        track.url = [NSURL URLWithString:filename];
        
        _streamer = [DOUAudioStreamer streamerWithAudioFile:track];
        [_streamer play];
    }
    else
    {
    
    }
}

- (void)Play
{
    if([_streamer status] == DOUAudioStreamerPaused)
    {
        [_streamer play];
    }
}

- (void)Pause
{
    if([_streamer status] == DOUAudioStreamerPlaying)
    {
        [_streamer pause];
    }
}

- (NSTimeInterval)GetCurrentPos
{
    return [_streamer currentTime];
}

- (NSTimeInterval)GetDuration
{
    return [_streamer duration];
}

- (double)GetVolume
{
    return [_streamer volume];
}

- (void)SetVolume:(double)value
{
    [_streamer setVolume:value];
}

- (void)Mute
{
    [_streamer setVolume:0.0];
}


@end
