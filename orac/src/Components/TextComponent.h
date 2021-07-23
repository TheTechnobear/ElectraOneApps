#pragma once

#include "Component.h"

class TextComponent : public Component
{
	public:

		TextComponent () :
			isDisabled (false),
			active (false),
			backgroundColour (0x0000)
		{
			*label = '\0';
		}

		~TextComponent ()
		{
		}

		// Lambdas
		std::function<bool (void)> onClick;

		void setLabel (const char *newLabel)
		{
			if (strlen (newLabel) == 0)
			{
				*label = '\0';
			}
			else
			{
				copyString (label, newLabel, maxLabelLength);
			}
			repaint();
		}

		void setBackgroundColour (uint16_t newBackgroundColour)
		{
			backgroundColour = newBackgroundColour;
		}

		void setDisabled (bool shouldBeDisabled)
		{
			isDisabled = shouldBeDisabled;
		}

		void setDimmed (bool shouldBeDimmed)
		{
		}

		bool isDimmed (void) const
		{
			return (false);
		}

		void setActive (bool shouldBeActive)
		{
			active = shouldBeActive;
		}

		bool isActive (void) const
		{
			return (active);
		}

		void paintUpdate (void)
		{
			paint ();
		}

		void paint (void)
		{
			uint16_t labelWidth = screen.getTextWidth (label,
				  TextStyle::smallWhiteOnBlack);

			screen.fillRect (screenX, screenY, width - 1, height - 1, backgroundColour);
			screen.printText (screenX, screenY, label, TextStyle::smallWhiteOnBlack,
			    width, TextAlign::center);

			if (active == true)
			{
				screen.drawLine (screenX + (width / 2) - (labelWidth / 2) - 1, screenY + 15,
								 labelWidth, 0, 0xFFFF);
			}
		}

		void onPotTouchDown (handle_t handle) override
		{
			if (onClick)
			{
				onClick ();
			}
		}

		void onPotTouchUp (handle_t handle) override
		{
		}

	private:
		static const uint8_t maxLabelLength = 20;
		char label[maxLabelLength + 1];
		uint16_t backgroundColour;

		struct
		{
			bool isDisabled : 1;
			bool active : 1;
		};
};
