<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <script src="<?php echo base_url(); ?>assets/jquery/jquery-3.7.1.min.js"></script>
    <script src="<?php echo base_url(); ?>assets/sweetalert/sweetalert.min.js"></script>
    <link rel="stylesheet" href="<?php echo base_url(); ?>assets/bootstrap/css/bootstrap.min.css" />
    <link rel="stylesheet" href="<?php echo base_url(); ?>assets/jquery/datatables.min.js" />
    <link rel="stylesheet" href="<?php echo base_url(); ?>assets/jquery/datatables.min.css" />
    <script src="<?php echo base_url(); ?>assets/bootstrap/js/bootstrap.min.js"></script>
</head>
<body>
    <!-- <div class="container" style="margin-top: 10% ;">
        <div class="card" style="width: 50% ;">
            <div class="card-body">
                <div class="form-group">
                    <label for="num_mat">Numero Matricule</label>
                    <input type="text" class="form-control" id="num_mat" placeholder="Numero Matricule">
                </div>
                <div class="form-group">
                    <label for="motdepass">Mot de Passe</label>
                    <input type="text" class="form-control" id="password" placeholder="Mot de Pass">
                </div>
            </div>

        </div>
    </div> -->

    <div class="container" style="margin-top: 15%">
        <div class="row">
            <?php echo validation_errors(); ?>
            <?php if(isset($login_fail)) echo $login_fail; ?>
            <div class="col-12 d-flex justify-content-center align-items-center">
                <div class="card" style="width: 50% ;">
                    <div class="card-body">
                        <form method="post" action="<?php echo site_url("Auth/login") ?>">
                            <div class="form-group">
                                <label for="num_mat">Numero Matricule</label>
                                <input type="text" class="form-control" id="num_mat" name="num_mat" placeholder="Numero Matricule" required>
                            </div>
                            <div class="form-group">
                                <label for="motdepass">Mot de Passe</label>
                                <input type="password" class="form-control" id="password" name="password" placeholder="Mot de Passe" required>
                            </div>
                            <div class="mt-5">
                                <button type="submit" class="btn btn-success col-lg-12" id="btn-connexion">Connexion</button>
                            </div>        
                        </form>
                    </div>
                </div>
            </div>
        </div>
    </div>


    
    
</body>
</html>