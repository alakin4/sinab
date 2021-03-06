
<!-- README.md is generated from README.Rmd. Please edit that file -->

# SINAB is not a browser

A basic html rendering engine for R, written in Rust. The purpose is not
to write a browser, but rather to provide the ability to render simple,
static html documents to an R graphics device.

## Installation

This package needs to be compiled from source and requires a working
Rust toolchain (i.e.,
[Cargo](https://doc.rust-lang.org/cargo/getting-started/installation.html)).
If you have Cargo up and running, the following should work to install
this package:

``` r
remotes::install_github("clauswilke/sinab")
```

## Examples

This project is in the early proof-of-concept stage. Expect most things
to be broken.

The main function provided by this package is `html_grob()`, which takes
as input some Markdown or HTML text and associated CSS and renders it
using the grid graphics system.

``` r
library(sinab)
library(grid)

mdtext <- "
Here are a few examples of word-wrapped and non-word-wrapped text. First
regular text pre-formatted:
<pre>
The quick brown
  fox jumps over
    the lazy dog.
</pre>
Now some inline code: `x <- 10; y <- 200; z <- 2*x + y;`
It gets wrapped.

We can also write block code. Notice how the long comment runs beyond
the box limits:

    x <- 10
    y <- 200
    z <- 2*x + y;  # and a really really long comment
"

css <- '
pre { background-color: #eee;
      padding: 6px;
      border-left: 5px solid #888; }
p   { background-color: #def;
      margin: 16px 0px 4px 0px;
      padding: 4px; }
code { border: 1px solid #aaa; 
       padding: 2px; }
pre code { border: none;
           padding: 0px; }
'

g <- html_grob(
  mdtext, css = css,
  x = unit(0.1, "npc"), y = unit(1, "npc"), width = unit(4, "inches"),
)
grid.newpage()
grid.draw(g)
```

<img src="man/figures/README-unnamed-chunk-3-1.png" width="100%" />

If the grob width is specified as a relative unit, then the grob is
reactive and reflows as the graphics window is resized. (Try this
example interactively.)

``` r
g <- html_grob(
  mdtext, css = css,
  x = unit(0.1, "npc"), y = unit(1, "npc"), width = unit(0.8, "npc"),
)
grid.newpage()
grid.draw(g)
```

<img src="man/figures/README-unnamed-chunk-4-1.png" width="100%" />

Simple markdown-to-html conversion is also implemented:

``` r
md_to_html("This is *a* **test**.")
#> [1] "<p>This is <em>a</em> <strong>test</strong>.</p>\n"
```

## FAQ

  - **Why is HTML/CSS feature X, Y, or Z not available?**  
    Rendering is done with a purpose-built layouting pipeline, and to
    date only a small subset of all possible features has been
    implemented.

  - **Will you support Javascript?**  
    Probably not. The goal for Sinab is to render static pages.
    Interactivity doesn’t work well with R graphics devices.

  - **Is MathML supported?**  
    Not at this time.

  - **Why is rendering so slow?**  
    The Sinab library itself is actually quite fast. Slowness comes
    mostly from R graphics devices. In particular, text shaping can be
    extremely slow. For faster rendering, try one of the graphics
    devices provided by the ragg library, such as `agg_png()`. The
    following benchmark highlights the importance of the graphics
    device. The agg device is 250 times faster than the quartz device\!
    All this extra time is spent text shaping.

<!-- end list -->

``` r
text <- paste(rep("Hello", 50), collapse = " ")

file <- tempfile(fileext = ".png")
png(file, width = 1920, height = 1920, res = 288, type = "quartz")
microbenchmark::microbenchmark(render_markdown(text), times = 10L)
#> Unit: milliseconds
#>                   expr      min       lq     mean  median       uq     max
#>  render_markdown(text) 677.8801 688.0131 700.9535 695.802 717.6722 727.584
#>  neval
#>     10
invisible(dev.off())

ragg::agg_png(file, width = 1920, height = 1920, res = 288)
microbenchmark::microbenchmark(render_markdown(text), times = 10L)
#> Unit: milliseconds
#>                   expr      min       lq     mean   median       uq      max
#>  render_markdown(text) 1.745371 1.908843 2.444105 2.002768 2.273038 6.195388
#>  neval
#>     10
invisible(dev.off())
```

  - **Why aren’t you supporting links (i.e., the `<a>` tag)?**  
    This is a limitation of the current R graphics device API. There is
    simply no way to create a link in an R graphics device. Once this
    feature gets added, it will be easy to support it in Sinab.

  - **Why do my colors look wrong?**  
    Sinab uses the color names defined by CSS, which in some cases are
    different from the color names that R uses. The most obvious example
    is probably “green”, which corresponds to \#00ff00 in R but \#008000
    in CSS. If you want to be certain that you get the colors you want,
    specify them with hex codes in RGB format.

## Acknowledgments

The actual HTML/CSS parsing and rendering code is written in Rust, and
it draws heavily from software developed for the Servo project. HTML
parsing is done with [html5ever](https://github.com/servo/html5ever),
CSS parsing is done with
[cssparser](https://github.com/servo/rust-cssparser), and CSS selectors
are implemented using the
[selectors](https://github.com/servo/servo/tree/master/components/selectors)
crate. DOM, layout, and rendering is using custom code that is in no
small part based on the experimental
[Victor](https://github.com/SimonSapin/victor) project written by Simon
Sapin.

Markdown to HTML conversion is performed through the
[pulldown-cmark](https://github.com/raphlinus/pulldown-cmark) crate,
which implements a lightweight Markdown parser that complies with the
[CommonMark spec.](https://spec.commonmark.org/)
