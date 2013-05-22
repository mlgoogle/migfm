//
//  PlayerViewController.m
//  carbonarrow
//
//  Created by Archer_LJ on 13-5-22.
//  Copyright (c) 2013年 Archer_LJ. All rights reserved.
//

#import "PlayerViewController.h"

static void *kStatusKVOKey = &kStatusKVOKey;
static void *kDurationKVOKey = &kDurationKVOKey;

// interface of track file
@interface Track : NSObject <DOUAudioFile>
@property (nonatomic, strong) NSString* artist;
@property (nonatomic, strong) NSString* title;
@property (nonatomic, strong) NSURL* url;
@end

@implementation Track

- (NSURL*)audioFileURL
{
    return [self url];
}

@end


// interface of music player
@interface PlayerViewController ()
{
@private
    DOUAudioStreamer* _streamer;
}
@end

@implementation PlayerViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
        _streamer = nil;
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)StartPlay:(NSString *)trackname
{
    // if trackname is a url
    if(1)
    {
        if(_streamer != nil)
        {
            [_streamer pause];
            [_streamer removeObserver:self forKeyPath:@"status"];
            [_streamer removeObserver:self forKeyPath:@"duration"];
            _streamer = nil;
        }
        
        Track* track = [[Track alloc]init];
        [track setUrl:[NSURL URLWithString:trackname]];
        //[track setArtist:@"Archer"];
        //[track setTitle:@"MySong"];
        
        _streamer = [DOUAudioStreamer streamerWithAudioFile:track];
        
        [_streamer addObserver:self forKeyPath:@"status" options:NSKeyValueObservingOptionNew context:kStatusKVOKey];
        [_streamer addObserver:self forKeyPath:@"duration" options:NSKeyValueObservingOptionNew context:kDurationKVOKey];
        
        [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(_timerAction:) userInfo:nil repeats:YES];
        
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

- (void)Stop
{
    [_streamer stop];
}

- (double)GetVolume
{
    return _streamer.volume;
}

- (void)SetVolume:(double)volume
{
    [_streamer setVolume:volume];
}

- (void)_timerAction:(id)timer
{
    // TODO: get play duration time
}

- (void)_updateStatus
{
    if(_streamer != nil)
    {
        switch ([_streamer status])
        {
            case DOUAudioStreamerPlaying:
                break;
                
            case DOUAudioStreamerPaused:
                break;
                
            case DOUAudioStreamerFinished:
                [self Stop];
                break;
                
            case DOUAudioStreamerBuffering:
                break;
                
            case DOUAudioStreamerError:
                [self Stop];
                break;
        }
    }
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    if (context == kStatusKVOKey) {
        [self performSelector:@selector(_updateStatus)
                     onThread:[NSThread mainThread]
                   withObject:nil
                waitUntilDone:NO];
    }
    else if (context == kDurationKVOKey) {
        [self performSelector:@selector(_timerAction:)
                     onThread:[NSThread mainThread]
                   withObject:nil
                waitUntilDone:NO];
    }
    else {
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
    }
}


@end
