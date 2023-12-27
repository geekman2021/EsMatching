<!DOCTYPE html>
<html lang="en" dir="ltr">
  <head>
    <meta charset="UTF-8" />
    <title>Rapprochement</title>

    <link rel="stylesheet" href="<?php echo base_url(); ?>assets/bootstrap/css/bootstrap.css" />
    <script src="<?php echo base_url(); ?>assets/jquery/jquery-3.7.1.min.js"></script>
    <script src="<?php  echo base_url(); ?>assets/jquery/jquery.dataTables.min.js"></script>
    <script src="<?php echo base_url(); ?>assets/moment.min.js"></script>
    <script src="<?php echo base_url(); ?>assets/jquery/dataTables.dateTime.min.js"></script>
    <script src="<?php echo base_url(); ?>assets/fr.js"></script>

    <script src="<?php echo base_url(); ?>assets/sweetalert.min.js"></script> 
    <script src="<?php echo base_url(); ?>assets/bootstrap/js/popper.min.js"></script>
    <script src="<?php echo base_url(); ?>assets/bootstrap/js/bootstrap.min.js"></script>
    <script src="<?php echo base_url(); ?>assets/chart/Chart.js"> </script>
    <link rel="stylesheet" type="text/css" href="<?php echo base_url(); ?>assets/jquery.dataTables.min.css" >
    <link rel="stylesheet" type="text/css" href="<?php echo base_url(); ?>assets/dataTables.dateTime.min.css" > 
    <link rel="stylesheet" href="<?php echo base_url(); ?>assets/jquery/datatables.css" />
    <link href="<?php echo base_url() ?>assets/boxicons/css/boxicons.min.css" rel="stylesheet" />
    <link rel="stylesheet" href="<?php echo base_url();?>assets/style.css">
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  
  </head>
  <body>
    <div class="sidebar">
      <div class="logo-details">
        <i class="bx bxl-c-plus-plus"></i>
        <span class="logo_name">Monétique</span>
      </div>
      <ul class="nav-links">
        <li>
          <a href="<?php echo site_url("Dashboard") ?>">
            <i class="bx bx-grid-alt"></i>
            <span class="link_name">Dashboard</span>
          </a>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("Dashboard") ?>">Dashboard</a></li>
          </ul>
        </li>
        <li>
        <?php if (isset($_SESSION["userExist"]) && isset($_SESSION["userExist"]->code_profil) && $_SESSION["userExist"]->code_profil ==="002") { ?>  
          <div class="iocn-link">
            <a href="<?php echo site_url("users") ?>">
              <i class="bx bx-collection"></i>
              <span class="link_name">Utilisateur</span>
            </a>
          </div>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("Users") ?>">Utilisateur</a></li>
          </ul>
        </li>
        <li>
          <div class="iocn-link">
            <a href="<?php echo site_url("Profiles") ?>">
              <i class="bx bx-book-alt"></i>
              <span class="link_name">Profil</span>
            </a>
          </div>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("Profiles") ?>">Profil</a></li>
          </ul>
        </li>
        <li>
          <!-- <div class="iocn-link">
            <a href="<?php echo site_url("droits") ?>">
              <i class="bx bx-book-alt"></i>
              <span class="link_name">Droit Utlisateur</span>
            </a>
          </div> -->
          <!-- <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("droits") ?>">Droit Utilisateur</a></li>
          </ul> -->
        </li>
        <li>
          <div class="iocn-link">
            <a href="#">
              <i class="bx bx-plug"></i>
              <span class="link_name">Operateur</span>
            </a>
            <i class="bx bxs-chevron-down arrow"></i>
          </div>
          <ul class="sub-menu">
            <li><a class="link_name" href="#">Operateur</a></li>
            <li><a href="<?php echo site_url("Airtel") ?>">Airtel</a></li>
            <li><a href="<?php echo site_url("Orange") ?>">Orange</a></li>
            <li><a href="<?php echo site_url("Telma") ?>">Telma</a></li>
          </ul>
        </li>

        <li>
          <a href="<?php echo site_url("Regul") ?>">
            <i class="bx bx-compass"></i>
            <span class="link_name">Regularisation</span>
          </a>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("Regul") ?>">Regularisation</a></li>
          </ul>
        </li>
        <li>
          <a href="<?php echo site_url("Historique") ?>">
            <i class="bx bx-history"></i>
            <span class="link_name">Rapprochement</span>
          </a>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("Historique") ?>">Rapprochement</a></li>
          </ul>
        </li>
      <?php } else if((isset($_SESSION["userExist"]) && isset($_SESSION["userExist"]->code_profil) && $_SESSION["userExist"]->code_profil ==="003")) { ?>
        <li>
          <a href="<?php echo site_url("Importer") ?>">
            <i class="bx bx-compass"></i>
            <span class="link_name">Importer</span>
          </a>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("Importer") ?>">Importer</a></li>
          </ul>
        </li>
        <li>
          <div class="iocn-link">
            <a href="#">
              <i class="bx bx-plug"></i>
              <span class="link_name">Operateur</span>
            </a>
            <i class="bx bxs-chevron-down arrow"></i>
          </div>
          <ul class="sub-menu">
            <li><a class="link_name" href="#">Operateur</a></li>
            <li><a href="<?php echo site_url("Airtel") ?>">Airtel</a></li>
            <li><a href="<?php echo site_url("Orange") ?>">Orange</a></li>
            <li><a href="<?php echo site_url("Telma") ?>">Telma</a></li>
          </ul>
        </li>

        <li>
          <a href="<?php echo site_url("Regul") ?>">
            <i class="bx bx-compass"></i>
            <span class="link_name">Regularisation</span>
          </a>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("Regul") ?>">Regularisation</a></li>
          </ul>
        </li>
        <li>
          <a href="<?php echo site_url("Historique") ?>">
            <i class="bx bx-history"></i>
            <span class="link_name">Rapprochement</span>
          </a>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("Historique") ?>">Rapprochement</a></li>
          </ul>
        </li>
        <li>
          <a href="<?php echo site_url("Ecart") ?>">
            <i class="bx bx-history"></i>
            <span class="link_name">Ecart</span>
          </a>
          <ul class="sub-menu blank">
            <li><a class="link_name" href="<?php echo site_url("Ecart") ?>">Ecart</a></li>
          </ul>
        </li>
      <?php } ?>
    </ul>
    </div>
    <section class="home-section">
      <div class="home-content">
        <div class="row" style="display: flex; justify-content: space-between; align-items: center;">
          <div>
            <i class="bx bx-menu"></i>
          </div>
          <div style="position: absolute; right: 0px;">
              <a class="nav-link dropdown-toggle" href="#" id="userDropdown" role="button"
                  data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
                  <span class="mr-2 d-none d-lg-inline text-gray-600 small"><?php echo $_SESSION["userExist"]->nom  ?></span>
                  <img class="img-profile rounded-circle" src="<?= base_url()?>assets/images/undraw_profile.svg" style="width: 40px; height: 40px;">
              </a>
              <div class="dropdown-menu dropdown-menu-right shadow animated--grow-in"
                  aria-labelledby="userDropdown">
                  <!-- <a class="dropdown-item" href="#">
                      <i class="bx bxs-user"></i>
                      Profile
                  </a>
                  <a class="dropdown-item" href="#">
                      <i class="bx bxs-cog"></i>
                      Paramètre
                  </a> -->
                  <!-- <div class="dropdown-divider"></div> -->
                  <a class="dropdown-item" href="#" data-toggle="modal" data-target="#logoutModal">
                      <i class='bx bxs-log-out'></i>
                      Deconnexion
                  </a>
              </div>
          </div>
        </div>
      </div>


<div class="modal fade" id="logoutModal">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <h6 class="modal-title">Êtes-vous sûr de vouloir vous déconnecter ?</h6>
          <button type="button" class="close" data-dismiss="modal">&times;</button>
        </div>

        <div class="modal-body">
          <p>Toute session en cours sera fermée, et vous devrez vous reconnecter pour accéder à nouveau à votre compte.</p>
        </div>
        <div class="modal-footer">
          <button type="button" class="btn btn-secondary" data-dismiss="modal">Annuler</button>
          <form action="<?php echo site_url("auth/logOut") ?>" method="post">
            <button type="submit" class="btn btn-danger">Deconnecter</button>
          </form>
        </div>
        
      </div>
    </div>
</div>



</html>
<script>
      let arrow = document.querySelectorAll(".arrow");
      for (var i = 0; i < arrow.length; i++) {
        arrow[i].addEventListener("click", (e) => {
          let arrowParent = e.target.parentElement.parentElement; //selecting main parent of arrow
          arrowParent.classList.toggle("showMenu");
        });
      }
      let sidebar = document.querySelector(".sidebar");
      let sidebarBtn = document.querySelector(".bx-menu");
      console.log(sidebarBtn);
      sidebarBtn.addEventListener("click", () => {
        sidebar.classList.toggle("close");
      });

</script>

