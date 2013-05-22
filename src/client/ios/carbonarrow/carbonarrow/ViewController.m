//
//  ViewController.m
//  carbonarrow
//
//  Created by Archer_LJ on 13-5-20.
//  Copyright (c) 2013年 Archer_LJ. All rights reserved.
//


#import "ViewController.h"

@interface ViewController ()
{
    PlayerViewController* _player;
}

@end

@implementation ViewController


- (void)viewDidLoad
{
    [super viewDidLoad];
    
    _player = [[PlayerViewController alloc]init];
    [_player StartPlay:@"ttp://zhangmenshiting.baidu.com/data2/music/35429401/15758731369008061128.mp3?xcode=6311e5ddb4b6e27db639b3d1b2fa946c"];

}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
