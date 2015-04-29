#include "HelpDialog.h"

HelpDialog::HelpDialog(QWidget* parent, Qt::WindowFlags f):
  QDialog(parent, f),
  layoutHelp      ( this ),
  labelHelp       ( this ),
  labelAbout      ( this ),
  textHelp        ( this ),
  closeHelpDialog ( this )
{
	setWindowTitle   ( "Loudness Validator - Help" );
	setMinimumWidth  ( 600 );
	setMinimumHeight ( 400 );
	setLayout        ( &layoutHelp );


	closeHelpDialog.setText("Close");
	closeHelpDialog.setIcon(QIcon(":/icons/dialog-close.png"));

	textHelp.setReadOnly( true );
	textHelp.setTextBackgroundColor( QColor(0,0,0) );
	textHelp.setHtml(
		"<p><center><h1>Loudness Validator</h1></center><p><br/>" \
		"<table><tr><td></td>" \
		"<td>In accordance with the recommendation R128 on the perceptual audio level.<br/><br/>" \
		"link: <a href=\"http://tech.ebu.ch/loudness\">http://tech.ebu.ch/loudness</a></td></tr>" \
		"<tr><td valign=\"bottom\"></td>" \
		"<td>Loudness Validator was an initiative from <b>MikrosImage</b>.<br/><br/><br/>" \
		"Lead developer and technical contact:<br/>" \
		"<b>Marc-Antoine ARNAUD</b><br/>" \
		"Engineer developer @ MikrosImage and @ Arkena<br/>" \
		"<a href=\"mailto:arnaud.marcantoine@gmail.com\">arnaud.marcantoine@gmail.com</a><br/><br/><br/>" \
		"Technical contact:<br/>" \
		"<b>Xavier BRACHET</b><br/>" \
		"Operations Manager and Head of Lab Department @ MikrosImage <br/>" \
		"<a href=\"mailto:xb@mikrosimage.eu\">xb@mikrosimage.eu</a><br/><br/></td></tr></table>"
	);

	layoutHelp.addWidget( &textHelp );
	layoutHelp.addWidget( &closeHelpDialog );

	connect( &closeHelpDialog, SIGNAL( pressed() ), this, SLOT( closeDialog() ) ) ;
}


void HelpDialog::closeDialog()
{
	accept();
}
