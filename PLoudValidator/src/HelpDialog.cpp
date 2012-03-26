#include "HelpDialog.h"

HelpDialog::HelpDialog(QWidget* parent, Qt::WindowFlags f):
  QDialog(parent, f),
  layoutHelp      ( this ),
  labelHelp       ( this ),
  labelAbout      ( this ),
  textHelp        ( this ),
  closeHelpDialog ( this )
{
	setWindowTitle   ( "PLoud Validator - Help" );
	setMinimumWidth  ( 600 );
	setMinimumHeight ( 400 );
	setLayout        ( &layoutHelp );


	closeHelpDialog.setText("Close");
	closeHelpDialog.setIcon(QIcon(":/icons/dialog-close.png"));

	textHelp.setReadOnly( true );
	textHelp.setTextBackgroundColor( QColor(0,0,0) );
	textHelp.setHtml(
		"<p><center><h1>Ploud Validator</h1><br/><h2>Version " \
		PLOUDVALIDATOR_VERSION \
		"</h2><br/>year : " \
		PLOUDVALIDATOR_YEAR \
		"<br/>month : "
		PLOUDVALIDATOR_MONTH \
		"<br/>day : "
		PLOUDVALIDATOR_DAY \
		"</center><p><br/>" \
		"<table><tr><td><img src=':/icons/ebur128.png'></img></td>" \
		"<td>In accordance with the recommendation R128 on the perceptual audio level.<br/><br/>" \
		"link: <a href=\"http://tech.ebu.ch/loudness\">http://tech.ebu.ch/loudness</a></td></tr>" \
		"<tr><td valign=\"bottom\"><img src=':/icons/logomikros.jpg'> </img></td>" \
		"<td>Ploud Validator was developped by <b>MikrosImage</b>.<br/><br/>" \
		"technical contact:<br/>" \
		"<b>Xavier BRACHET</b><br/>" \
		"Operations Manager and Head of Lab Department @ MikrosImage <br/>" \
		"<a href=\"mailto:xb@mikrosimage.eu\">xb@mikrosimage.eu</a><br/><br/>" \
		"developer contact:<br/>" \
		"<b>Marc-Antoine ARNAUD</b><br/>" \
		"Engineer developer @ MikrosImage <br/>" \
		"<a href=\"mailto:mrn@mikrosimage.eu\">mrn@mikrosimage.eu</a></td></tr></table>"
	);

	layoutHelp.addWidget( &textHelp );
	layoutHelp.addWidget( &closeHelpDialog );

	connect( &closeHelpDialog, SIGNAL( pressed() ), this, SLOT( closeDialog() ) ) ;
}


void HelpDialog::closeDialog()
{
	accept();
}
