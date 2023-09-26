n to the application from phpMyAdmin is
through scanning a QR code.</p>
<p>There are dozens of applications available for mobile phones to implement these
standards, the most widely used include:</p>
<ul class="simple">
<li><a class="reference external" href="https://freeotp.github.io/">FreeOTP for iOS, Android and Pebble</a></li>
<li><a class="reference external" href="https://authy.com/">Authy for iOS, Android, Chrome, OS X</a></li>
<li><a class="reference external" href="https://apps.apple.com/us/app/google-authenticator/id388497605">Google Authenticator for iOS</a></li>
<li><a class="reference external" href="https://play.google.com/store/apps/details?id=com.google.android.apps.authenticator2">Google Authenticator for Android</a></li>
<li><a class="reference external" href="https://lastpass.com/auth/">LastPass Authenticator for iOS, Android, OS X, Windows</a></li>
</ul>
</div>
<div class="section" id="hardware-security-key-fido-u2f">
<h2>Hardware Security Key (FIDO U2F)<a class="headerlink" href="#hardware-security-key-fido-u2f" title="Permalink to this headline">¶</a></h2>
<p>Using hardware tokens is considered to be more secure than a software based
solution. phpMyAdmin supports <a class="reference external" href="https://en.wikipedia.org/wiki/Universal_2nd_Factor">FIDO U2F</a>
tokens.</p>
<p>There are several manufacturers of these tokens, for example:</p>
<ul class="simple">
<li><a class="reference external" href="https://www.yubico.com/fido-u2f/">youbico FIDO U2F Security Key</a></li>
<li><a class="reference external" href="https://www.hypersecu.com/tmp/products/hyperfido">HyperFIDO</a></li>
<li><a class="reference external" href="https://trezor.io/?offer_id=12&amp;aff_id=1592&amp;source=phpmyadmin">Trezor Hardware Wallet</a> can act as an <a class="reference external" href="https://wiki.trezor.io/User_manual:Two-factor_Authentication_with_U2F">U2F token</a></li>
<li><a class="reference external" href="https://www.dongleauth.info/dongles/">List of Two Factor Auth (2FA) Dongles</a></li>
</ul>
</div>
<div class="section" id="simple-two-factor-authentication">
<span id="simple2fa"></span><h2>Simple two-factor authentication<a class="headerlink" href="#simple-two-factor-authentication" title="Permalink to this headline">¶</a></h2>
<p>This authentication is included for testing and demonstration purposes only as
it really does not provide two-factor authentication, it just asks the user to confirm login by
clicking on the button.</p>
<p>It should not be used in the production and is disabled unless
<span class="target" id="index-0"></span><a class="reference internal" href="config.html#cfg_DBG_simple2fa"><code class="xref config config-option docutils literal notranslate"><span class="pre">$cfg['DBG']['simple2fa']</span></code></a> is set.</p>
</div>
</div>


          </div>
        </div>
      </div>
      <div class="sphinxsidebar" role="navigation" aria-label="main navigation">
        <div class="sphinxsidebarwrapper">
  <h3><a href="index.html">Table of Contents</a></h3>
  <ul>
<li><a class="reference internal" href="#">Two-factor authentication</a><ul>
<li><a class="reference internal" hr