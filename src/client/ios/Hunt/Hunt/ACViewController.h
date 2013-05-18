//
//  ACViewController.h
//  Hunt
//
//  Created by Archer_LJ on 13-3-2.
//  Copyright (c) 2013年 Archer_LJ. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ACViewController : UIViewController<UIWebViewDelegate>
{
    IBOutlet UIWebView *_mainWebView;
    IBOutlet UINavigationBar *_mainTitleBar;
}
- (IBAction)_pageBack:(UIBarButtonItem *)sender;
- (IBAction)_pageReload:(id)sender;

- (void)webViewDidFinishLoad:(UIWebView *)webView;

- (void)doHuntJob;

@end
