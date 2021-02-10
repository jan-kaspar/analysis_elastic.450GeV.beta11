import root;
import pad_layout;
include "../common.asy";

string dataset = "data/fill7291/Totem1";

string f = topDir + dataset + "/alignment_final.root";

TH2_palette = Gradient(blue, heavygreen, red, black);

//----------------------------------------------------------------------------------------------------

NewPad("$\th^*_x\ung{\mu rad}$", "$\th^*_y\ung{\mu rad}$");

TH2_zLabel = "$|\hbox{hist} - \hbox{fit}| / \hbox{unc}$";

draw(scale(1e6, 1e6), RootGetObject(f, "diff map"), "def");
