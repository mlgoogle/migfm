//
//  ACViewController.m
//  carbonarrow
//
//  Created by Archer_LJ on 13-5-18.
//  Copyright (c) 2013年 Archer_LJ. All rights reserved.
//

#import "ACViewController.h"
#import "ACArrowPlayer.h"

@interface ACViewController ()

@end

@implementation ACViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    ACArrowPlayer* player = [[[ACArrowPlayer alloc]init]autorelease];
    [player InitPlayer:@"http://douban.fm/j/mine/playlist?type=n&channel=1&from=mainsite"];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
