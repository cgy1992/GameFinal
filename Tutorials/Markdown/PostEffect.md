# Post Effect #

Post effect is a kind of image processing. In essence, it is an color transformation for every pixel in the screen space. Each post effect in GameFinal corresponds to one **compositor** object. All the compositors must implement the ICompositor interface. If developers want to design their own post effect, they just need define a new class that inherits from ICompositor and implement the abstract methods. One compositor could be connected with another compositor, thus several compositors could be combined as a list. Just as the following picture shows, the output of the current compositor is the input of the next compositor:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/10-04.png)

You can add multiple compositors into your program. Be aware that the order in which you add them into the scene manager will affect the final effect on the screen:

	smgr->addCompositor(compositor1);
	smgr->addCompositor(compositor2);
	smgr->addCompositor(compositor3);


We can define our post effect by creating a new class that implements the `ICompositor` interface:

	class MyCompositor : public ICompositor
	{
		virtual void render(ISceneManager* smgr, bool lastCompositor) 
		{
			// TODO: Do your post effect's processing
		}
	};

The only pure virtual method in ICompositor is `render` method. Your class must implement this abstract method. It receives two parameters: the first one is the scene manager; the second parameter `lastCompositor` indicates whether this compositor is the last one in the compositor chain. If it is the last one, you might like writing the rendering result directly into the backbuffer rather than a texture render target.


## Blur ##

GameFinal provides us with several built-in post effects. We can add them directly in our program without writing too complicated code. Firstly, let's try the blurring effect. In order to create a built-in compositor, we call `ISceneManager::createCompositor` method which returns a `ICompositor` inferface:

	SCompositorCreateParam param;
	param.Blur.Algorithm = SCompositorCreateParam::EBLUR_GAUSSIAN;
	param.Blur.TexelDistance = 2.0f;
	param.Blur.PassCount = 5;

	ICompositor* compositor = smgr->createCompositor(ECT_BLUR, param);
	smgr->addCompositor(compositor);

Firstly, we initialize a `SCompositorCreateParam` structure. If we want to create a blur effect, we need to fill its `Blur` attribute. The `param.Blur.Algorithm` specifies which blur algorithm we want to use. Here we set it `SCompositorCreateParam::EBLUR_GAUSSIAN`, which means we would like to use gaussian blur. The `TexelDistance` indicates the distance between neighbouring texels that we'll blur. The `PassCount` means how many blur passes there would be in the compositor. The larger it is, the more fuzzy the final image will be.

Then we call the `ISceneManager::createCompositor` to create the `ICompositor` interface. Notice the first parameter indicates what kind of post effect you would like to create. Here we set it to `ECT_BLUR`, which means we'll create a blur effect. Then we call `addCompositor` to add this compositor to the chain.

Run the program, we could find the whole screen becomes fuzzy:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/10-02.png)

## Bloom ##

Bloom is another post effect that changes light and shade contrast of image. When a pixel is illuminated by a very bright light, it will extend its brightness to its neighbouring pixels. To create a bloom effect, we need to care more attributes in the `SCompositorCreateParam` structure:

	SCompositorCreateParam param;
	param.Bloom.BlurPassCount = 4;
	param.Bloom.BlurTexelDistance = 2.0f;
	param.Bloom.BrightnessThreshold = 0.7f;
	param.Bloom.BlurTextureWidth = 400;
	param.Bloom.BlurTextureHeight = 300;

	ICompositor* compositor = smgr->createCompositor(ECT_BLOOM, param);
	smgr->addCompositor(compositor);

The `BlurPassCount` attribute indicates the number of blur passes. `BlurTexelDistance` has the same meaning in Blur effect. `BrightnessThreshold` gives a threshold: the engine would only highlight the region whose brightness is above this threshold value. `BlurTextureWidth` and `BlurTextureHeight` specifies the size of temporary render target used in blurring process.

Run the program, you would find those very bright regions would affect the pixels around them:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/10-01.png)

## Motion Blur ##

If the camera moves fast enough, the image that projects into player’s eye would become blurred. This phenomenon is called "motion blur". Motion blur is widely used in video games as well as movies in order to increase realism.

The following code shows how to add motion blur:

	SCompositorCreateParam param;
	param.MotionBlur.SampleNum = 10;

	ICompositor* compositor = smgr->createCompositor(ECT_MOTION_BLUR, param);
	smgr->addCompositor(compositor);

The only attribute we need care about is `SampleNum`, which affects how blurred the image should be. The larger this value is set, the more player's movements would affect the screen's blurness.

Run the program, while you control the FPS camera frequently in a high speed, we'll notice the phenomenon:

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/10-03.png)